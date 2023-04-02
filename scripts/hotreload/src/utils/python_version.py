import os
import sys
import shutil


def ensure_valid_python_version():
    # If the version of Python is too old, attempt to locate a suitable version,
    # otherwise simply fail.
    if sys.version_info[0] <= 3 and sys.version_info[1] < 11:
        # We only search for python3.11 as searching for every potential version is
        # silly.

        # If a python3.11 command exists, we assume the user intended to run with
        # this version.
        python311 = shutil.which('python3.11')
        if python311:
            # Print warnings.
            print("The hot-reload toolchain requires a Python version of at least 3.11.", file=sys.stderr)
            print(f"You are running {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}.", file=sys.stderr)

            # Then, rerun with the detected version.
            print("", file=sys.stderr)
            print(f"Successfully located Python 3.11 at {python311}. Restarting with Python 3.11...", file=sys.stderr)
            print("", file=sys.stderr)

            # Executes a new program, replacing the current process, and does not return.
            sys.stderr.flush()
            sys.stdout.flush()
            os.execv(python311, [python311, os.path.realpath(sys.argv[0])] + sys.argv[1:])
        else:
            raise Exception(
                f"The hot-reload toolchain requires a Python version of at least 3.11. You are running {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}."
            )
