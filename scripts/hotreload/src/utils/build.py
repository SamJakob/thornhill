import os
import signal
import subprocess
import sys
from multiprocessing import Process

from pathlib import Path
from threading import Thread
from typing import Optional, Self, Callable
from subprocess import Popen

from .loaders import LoaderStyle, Loader


def pipe_output(source, sink):
    def do_pipe_output():
        for line in iter(source.readline, ""):
            sink.write(line.decode())

    thread = Thread(target=do_pipe_output, daemon=True)
    thread.start()
    return thread


class Builder:

    def __init__(self, command: str | list[str], working_dir: str | Path, on_complete: Optional[Callable] = None):
        self.started = False

        self.command = command
        self.working_dir = working_dir

        self.on_complete = on_complete

        self.__threads = []

        self.__build_process: Optional[Popen] = None
        self.__build_loader: Optional[Loader] = None

        self.__handling_completed = False

        self.__invocation = 0

    def run(self, message: str = "Building, please wait...", loader_style: LoaderStyle = LoaderStyle.SPINNER) -> Self:
        if self.started:
            # Interrupt and start again.
            self.interrupt()

        self.started = True
        self.__invocation = self.__invocation + 1

        self.__build_loader = Loader(loader_style).start(message)

        build_process_env = os.environ.copy()
        build_process_env['CLICOLOR_FORCE'] = '1'

        self.__build_process: Optional[Popen] = Popen(self.command,
                                                      stdout=subprocess.PIPE,
                                                      stderr=subprocess.PIPE,
                                                      shell=True, cwd=self.working_dir,
                                                      env=build_process_env)

        self.__threads = [pipe_output(self.__build_process.stdout, self.__build_loader.get_proxy(sys.__stdout__)),
                          pipe_output(self.__build_process.stderr, self.__build_loader.get_proxy(sys.__stderr__))]

        def watcher_task(watcher_invocation: int):
            self.__build_process.wait()

            if self.__invocation == watcher_invocation:
                self.__handle_complete()

        Thread(target=watcher_task, daemon=True, args=[self.__invocation]).start()
        return self

    def __handle_complete(self):
        if not self.started:
            return

        if self.__handling_completed:
            return

        self.__handling_completed = True

        for thread in self.__threads:
            thread.join(timeout=0)

        self.__threads = []

        time_taken_seconds = self.__build_loader.stop()

        print(f"\033[1;92m(\u2714) Build completed! ({time_taken_seconds}s)\033[0m")
        self.started = False
        self.__handling_completed = False

        if self.on_complete is not None:
            self.on_complete()

    def wait(self):
        if not self.started:
            return

        self.__build_process.wait()
        self.__handle_complete()

    def interrupt(self, message="Build interrupted!"):
        if not self.started:
            return

        self.started = False
        self.__build_process.terminate()
        self.__build_process = None

        for thread in self.__threads:
            thread.join(timeout=0)

        self.__threads = []

        self.__build_loader.message = "Interrupted!"
        time_taken_seconds = self.__build_loader.stop()

        print(f"\033[1;91m(!) {message} ({time_taken_seconds}s)\033[0m")

    def run_and_wait(self, message: str = "Building, please wait..."):
        self.run(message).wait()
