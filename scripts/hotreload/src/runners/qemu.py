import json
import subprocess
import sys
import time
from json import JSONDecodeError

from pathlib import Path
from subprocess import Popen
from threading import Thread
from typing import Self, Optional, Literal
from socket import socket, AF_INET, SOCK_STREAM

from . import BaseRunner
from ..utils.loaders import Loader, LoaderStyle

MAGIC_TH_DEBUGGER_SENTINEL = 0x534A4142

MAX_CONNECT_TIMEOUT = 5
"""Maximum timeout, in seconds, before connecting to server."""


class QEMURunner(BaseRunner):

    @staticmethod
    def register(**kwargs):
        BaseRunner.register('qemu', QEMURunner)

    def __init__(self, working_dir: str | Path, start_command: str, port: int, _symbols: dict[str, str]):
        super().__init__()

        self.working_dir = working_dir
        """The directory commands are executed in."""

        self.start_command = start_command
        """The command used to start QEMU."""

        self.port = port
        """The QMP port."""

        self.__qemu_process: Optional[Popen] = None
        self.__qmp_socket: Optional[socket] = None

        self.__qmp_connected = False

        self.__symbols = _symbols
        self.__queue = []

    def start(self) -> Self:
        start_loader = Loader(LoaderStyle.SPINNER).start("Booting QEMU runner...")

        self.__qemu_process: Optional[Popen] = Popen(self.start_command,
                                                     shell=True,
                                                     stdout=subprocess.PIPE,
                                                     stderr=subprocess.PIPE,
                                                     cwd=self.working_dir)

        # ATTEMPT TO CONNECT TO QMP SOCKET.

        def attempt_connect():
            waited = 0

            while waited < 10:
                try:
                    self.__qmp_socket = socket(AF_INET, SOCK_STREAM)
                    self.__qmp_socket.connect(('localhost', self.port))
                    break
                except ConnectionRefusedError:
                    waited += 0.5
                    time.sleep(0.5)

            self.__qmp_socket.setblocking(True)     # Set socket as blocking.
            self.__qmp_socket.settimeout(5)         # Set socket timeout to 5 seconds.

            while not self.__qmp_connected:
                # Wait for the 'hello' packet.
                recv = self.recv_command()

                # If we get a packet that resembles the 'hello' packet,
                if 'QMP' in recv and 'capabilities' in recv['QMP']:

                    # Send the qmp_capabilities handshake packet to enter
                    # command mode.
                    self.send_command({"execute": "qmp_capabilities"})

                    # Wait for a reply.
                    while 'return' not in self.recv_command():
                        pass

                    # When we get it mark as connected.
                    self.__qmp_connected = True

        attempts = 0
        while not self.__qmp_connected:
            try:
                attempt_connect()
            except:
                print("\033[1;93m(!) There was a problem connecting to QEMU. Trying again...\033[0m")
                time.sleep(1)

                if attempts > 3:
                    print(f"\033[1;91m(!) Failed to connect to QEMU runner after 3 attempts.\033[0m")
                    exit(1)

                attempts += 1

        # ATTEMPT TO LOCATE DEBUG SENTINEL AND WAIT FOR HAS_BOOTED TO BECOME TRUE.
        start_loader.stop()

        boot_loader = Loader(LoaderStyle.INDETERMINATE_BAR).start("Locating debugging region...")

        enhanced_debugging = False
        enhanced_debugging_guest = None

        # Locate the Debugger Sentinel value to determine if debugging is possible.
        if 'TH_DEBUGGER_SENTINEL' in self.__symbols and self.__symbols['TH_DEBUGGER_SENTINEL'] is not None:
            debugger_sentinel_response: Optional[int] = None
            attempt = 1

            while debugger_sentinel_response is None:
                if attempt >= 30:
                    print("\033[1;91m(!) Either the debugging region is not active,")
                    print("or the machine did not become ready in time.\033[0m")
                    break

                debugger_sentinel_response = self.dump_symbol(self.__symbols['TH_DEBUGGER_SENTINEL'])
                attempt += 1
                time.sleep(1)

            if debugger_sentinel_response is not None and debugger_sentinel_response == MAGIC_TH_DEBUGGER_SENTINEL:
                enhanced_debugging = True

        boot_loader.message = "Waiting for Thornhill to boot..."

        # Next, wait for HAS_BOOTED to be set.
        if enhanced_debugging:
            if 'HAS_BOOTED' in self.__symbols and self.__symbols['HAS_BOOTED'] is not None:
                has_booted_response: Optional[int] = None
                while has_booted_response != 1:
                    has_booted_response = self.dump_symbol(self.__symbols['HAS_BOOTED'])
                    time.sleep(1)

            # Then, get the guest version information.
            version_identifier_length = self.dump_symbol(self.__symbols['TH_VERSION_IDENTIFIER_LEN'])
            version_identifier_ptr = self.dump_symbol(self.__symbols['TH_VERSION_IDENTIFIER'])

            enhanced_debugging_guest = self.dump_symbol(hex(version_identifier_ptr),
                                                        value_type='chars',
                                                        length=version_identifier_length)
        else:
            time.sleep(5)

        boot_loader.stop()

        if enhanced_debugging:
            print()
            print("\033[1;92m(\u2714) Enhanced debugging is active!\033[0m")

            if enhanced_debugging_guest is not None:
                print(f"\033[1;92m(\u2714) \tGuest OS: {enhanced_debugging_guest}\033[0m")

            print()
            print("\033[1;92m(\u2714) System booted!\033[0m")
        else:
            print(f"\033[1;91m(!) Enhanced debugging disabled.\n"
                  f"(!) Either the guest is not supported, or your system does not have the necessary tools.\033[0m")

        return self

    def restart(self):
        print("\033[1;93m(!) Issued system reset command to QEMU...\033[0m")

        # Send the block_stream command. Wait for a "concluded" status and issue
        # the system_reset command.
        self.send_command({"execute": "human-monitor-command", "arguments": {
            "command-line": "block_stream virtio0"
        }})

        recv = None
        # Could also check for JOB_STATUS_CHANGE = concluded, but this seems easier.
        while recv is None or "event" not in recv or recv["event"] != "BLOCK_JOB_COMPLETED":
            recv = self.recv_command()

        self.send_command({"execute": "system_reset"})

    def send_command(self, payload: dict):
        if self.__qmp_socket is None:
            print(f"\033[1;91m(!) Send command failed. No QMP socket is currently set up.\033[0m")
            return

        raw_payload = json.dumps(payload)
        self.__qmp_socket.send(raw_payload.encode(encoding='utf-8'))

    def has_waiting(self) -> bool:
        return len(self.__queue) > 0

    def flush_queue(self):
        self.__queue.clear()

    def recv_command(self) -> any:
        if self.has_waiting():
            return json.loads(self.__queue.pop())

        try:
            recv = self.__qmp_socket.recv(65535).decode(encoding='utf-8').splitlines()
        except TimeoutError:
            return None

        # Take the first command and execute it now.
        now = recv.pop()

        # If multiple commands were received (OOB), then place them in the queue
        # for later invocations.
        if len(recv) > 0:
            for payload in recv:
                self.__queue.append(payload)

        # Now, process the latest message.
        return json.loads(now)

    def rpc_command(self, payload: dict) -> any:
        self.send_command(payload)
        recv = None

        while recv is None or 'return' not in recv:
            recv = self.recv_command()

        return recv['return']

    def dump_symbol(self,
                    addr: str | int,
                    value_type: Literal['hex', 'chars'] = 'hex',
                    length: Optional[int] = None) -> Optional[int | str]:

        # Normalize the address.
        if type(addr) is str:
            addr = int(addr, 16)
        addr = hex(addr).upper()

        # Create the command. Request a single hex word if format is just 'hex',
        # otherwise handle the format specifier.
        command_str = f"x/x {addr}"

        # Request characters byte-by-byte in hex (for more compact representation).
        if value_type == 'chars':
            if length is None:
                raise ValueError("You must specify the length when value_type is chars.")

            command_str = f"x/{length}xb {addr}"

        # Issue the command with human-monitor-command.
        result = self.rpc_command({"execute": "human-monitor-command", "arguments": {
            "command-line": command_str
        }})

        # Parse the response byte-by-byte back into a string.
        if value_type == 'chars':
            def read_chars():
                lines = result.split('\n')

                requested_addr = int(addr, 16)
                end_addr = requested_addr + length

                accumulator = bytearray()

                for line in lines:
                    if len(line.strip()) < 1:
                        continue

                    line_parts = line.split(": ")
                    base_addr = int(line_parts[0], 16)
                    line_bytes = line_parts[1].split(" ")

                    current_address = base_addr

                    for byte in line_bytes:
                        # Skip over bytes that were not requested.
                        if current_address < requested_addr:
                            current_address += 1
                            continue

                        # Accumulate the bytes as they are read.
                        accumulator.append(int(byte, 16))

                        # If we reach the end of the requested section, return the accumulator value.
                        if current_address >= end_addr - 1:
                            return accumulator

                        current_address += 1

                return None

            payload = read_chars()

            if payload is not None:
                return payload.decode('utf-8')
            else:
                return None

        try:
            return int(result.split(":")[1].strip(), 16)
        except ValueError:
            return None
