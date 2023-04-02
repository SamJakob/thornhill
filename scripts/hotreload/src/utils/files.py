# Helper methods to locate relevant files and directories.
import os.path
import tomllib
import json

from pathlib import Path
from typing import Optional


def locate_project_root_dir() -> Path:
    """
    Locates the root of the project by recursively searching upwards from the directory
    containing the script until a file called .root is found.

    This, naturally, assumes that the hotreload scripts are somewhere located within the
    project root.
    """
    check_dir = Path(os.path.dirname(os.path.realpath(__file__)))
    while not os.path.isfile(check_dir.joinpath('.root')):
        # If we've reached the root directory,
        # raise an error (project root not found).
        if check_dir == check_dir.parent:
            raise Exception("Project root could not be found.")

        check_dir = check_dir.parent

    # Otherwise, we've found the project root directory.
    return check_dir


def locate_cmake_build_dir(base: Path = None) -> Path:
    # Attempt to locate the project root automatically if not specified.
    if base is None:
        base = locate_project_root_dir()

    # Locate the first directory containing .cmake.
    try_cmake_build_dir: Optional[Path] = None

    for path in base.rglob('.cmake/'):
        # noinspection PyBroadException
        try:
            cmake_build_dir = path.parent

            # Set the fallback cmake build directory to EITHER:
            # - the first cmake directory found, OR
            # - the most recent cmake build directory found (if there are multiple)
            if try_cmake_build_dir is None or try_cmake_build_dir.stat().st_mtime_ns < cmake_build_dir.stat().st_mtime_ns:
                try_cmake_build_dir = cmake_build_dir

            # Attempt to automatically detect and verify a valid CMake directory.
            cmake_api_reply_dir = path.joinpath('api', 'v1', 'reply')
            if cmake_api_reply_dir.is_dir():
                for toolchains_reply_file in cmake_api_reply_dir.rglob('toolchains-v1-*.json'):
                    toolchains_reply = json.loads(toolchains_reply_file.read_text(encoding='utf-8'))

                    # If the compiler within a toolchain exists on the current system, we'll use that
                    # CMake directory.
                    for toolchain in toolchains_reply['toolchains']:
                        if Path(toolchain['compiler']['path']).exists():
                            return cmake_build_dir
        except Exception:
            pass

    if try_cmake_build_dir is not None:
        return try_cmake_build_dir
    else:
        raise Exception(
            "Failed to locate a valid CMake build directory.\n"
            "Please ensure you have loaded the CMake project and the .cmake/api directory "
            "exists within your cmake build directory."
        )


def load_config(config_filename: str, base: Path = None):
    # Attempt to locate the project root automatically if not specified.
    if base is None:
        base = locate_project_root_dir()

    # Ensure the config file exists.
    config_file = base.joinpath(config_filename)
    if not config_file.is_file():
        raise Exception(f"Project configuration file ({config_filename}) not found in {base}.")

    # Load the contents of the config file.
    return tomllib.loads(config_file.read_text(encoding='utf-8'))
