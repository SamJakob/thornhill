import json
import subprocess
import time

from pathlib import Path
from subprocess import Popen
from threading import Thread
from typing import Self, Optional, Literal
from socket import socket, AF_INET, SOCK_STREAM

from . import BaseRunner
from ..utils.loaders import Loader, LoaderStyle


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

    def start(self) -> Self:
        start_loader = Loader(LoaderStyle.SPINNER).start("Booting QEMU runner...")

        self.__qemu_process: Optional[Popen] = Popen(self.start_command,
                                                     shell=True,
                                                     stdout=subprocess.PIPE,
                                                     stderr=subprocess.PIPE,
                                                     cwd=self.working_dir)

        time.sleep(2)

        # ATTEMPT TO CONNECT TO QMP SOCKET.

        def attempt_connect():
            self.__qmp_socket = socket(AF_INET, SOCK_STREAM)
            self.__qmp_socket.connect(('localhost', self.port))

            self.__qmp_socket.setblocking(True)

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
                print("\033[1;93m(!) There was a problem starting QEMU. Trying again...\033[0m")

                if attempts > 3:
                    print(f"\033[1;91m(!) Failed to start QEMU runner after 3 attempts.\033[0m")
                    exit(1)

                attempts = attempts + 1

        # ATTEMPT TO LOCATE DEBUG SENTINEL AND WAIT FOR HAS_BOOTED TO BECOME TRUE.
        start_loader.stop()

        boot_loader = Loader(LoaderStyle.INDETERMINATE_BAR).start("Waiting for Thornhill to boot...")

        while True:
            res = (self.dump_symbol(self.__symbols['TH_DEBUGGER_SENTINEL']))
            print(res)
            if 'Cannot' not in res:
                break
            time.sleep(1)

        boot_loader.stop()

        print("\033[1;92m(\u2714) System booted!\033[0m")

        return self

    def restart(self):
        print("\033[1;93m(!) Issued system reset command to QEMU...\033[0m")
        self.send_command({"execute": "system_reset"})

    def send_command(self, payload: dict):
        if self.__qmp_socket is None:
            print(f"\033[1;91m(!) Send command failed. No QMP socket is currently set up.\033[0m")
            return

        raw_payload = json.dumps(payload)
        self.__qmp_socket.send(raw_payload.encode(encoding='utf-8'))

    def recv_command(self) -> dict:
        return json.loads(self.__qmp_socket.recv(65535).decode(encoding='utf-8'))

    def rpc_command(self, payload: dict) -> dict:
        self.send_command(payload)
        recv = self.recv_command()

        while 'return' not in recv:
            pass

        return recv['return']

    def dump_symbol(self, addr: str, format: Literal['hex', 'chars'] = 'hex', len: Optional[int] = None):
        command_str = f"x/x 0x{addr.strip()}"

        if format == 'chars':
            if len is None:
                raise ValueError("You must specify len when format is chars.")

            command_str = f"x/{len}c 0x{addr}"

        return self.rpc_command({"execute": "human-monitor-command", "arguments": {
            "command-line": command_str
        }})
