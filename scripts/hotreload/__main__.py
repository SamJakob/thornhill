#!/usr/bin/env python3
# encoding: utf-8
import sys

from os.path import dirname
from pathlib import Path

sys.path.append(dirname(sys.path[0]))

from src.utils.python_version import ensure_valid_python_version
ensure_valid_python_version()

from src.utils.build import Builder
from src.utils.files import load_config, locate_project_root_dir, locate_cmake_build_dir
from src.utils.loaders import LoaderStyle, load_while
from src.utils.watcher import Watcher
from src.utils.elf import locate_elf_symbol

from src.runners import BaseRunner
from src.runners.qemu import QEMURunner


def main():
    QEMURunner.register()

    project_root = locate_project_root_dir()
    config = load_config("thornhill-hot-toolchain.toml", base=project_root)

    # Ensure selected runner is registered.
    if not BaseRunner.has_name(config['runner']['type']):
        print(f"The selected runner ({config['runner']['type']}) is not a recognized runner.")
        sys.exit(1)

    # Locate the CMake build directory.
    cmake_root: Path = load_while(
        lambda: locate_cmake_build_dir(base=project_root),
        style=LoaderStyle.INDETERMINATE_BAR,
        message="Locating CMake build directory..."
    )

    # Builder(command=config['watch']['build'], working_dir=cmake_root)\
    #     .run_and_wait("Performing initial build...")

    # Identify the TH_DEBUGGER_SENTINEL and HAS_BOOTED memory locations.
    def locate_kernel_symbol(name: str):
        return load_while(
            lambda: locate_elf_symbol(cmake_root.joinpath('kernel', 'kernel'), name),
            style=LoaderStyle.INDETERMINATE_BAR,
            message=f"Locating {name}"
        )

    _symbols = {
        "TH_DEBUGGER_SENTINEL": locate_kernel_symbol('TH_DEBUGGER_SENTINEL'),
        "HAS_BOOTED": locate_kernel_symbol('HAS_BOOTED'),
        "TH_VERSION_IDENTIFIER_LEN": locate_kernel_symbol('TH_VERSION_IDENTIFIER_LEN'),
        "TH_VERSION_IDENTIFIER": locate_kernel_symbol('TH_VERSION_IDENTIFIER'),
    }

    runner = BaseRunner.for_name(
        config['runner']['type'],
        working_dir=cmake_root,
        start_command=config['runner']['start'],
        port=config['runner']['port'],
        _symbols=_symbols
    ).start()

    def on_rebuild():
        runner.restart()
        print("\033[1;92m(\u2714) Watching for changes...\033[0m")

    rebuilder = Builder(
        command=config['watch']['rebuild'],
        working_dir=cmake_root,
        on_complete=on_rebuild
    )

    Watcher(
        paths=config['watch']['targets'],
        base=project_root,
        on_change=lambda: rebuilder.run("Rebuilding project...", loader_style=LoaderStyle.INDETERMINATE_BAR)
    ).run()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("")
        print("Exiting...")
        sys.exit(130)
