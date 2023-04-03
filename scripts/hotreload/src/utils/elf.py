import os
import shutil
import subprocess
from pathlib import Path
from typing import Optional


def locate_elf_symbol(file: Path, symbol_name: str) -> Optional[str]:
    """
    Locates the hexadecimal memory address of the specified symbol, in the file,
    if it can be found.
    """

    # Attempt to locate readelf.
    readelf = shutil.which('readelf')

    if readelf is None:
        readelf = shutil.which('readelf', path=os.environ.get('TH_TC_PATH'))

    if readelf is None:
        return None

    # Attempt to locate the file.
    if not file.exists():
        return None

    # Attempt to call 'readelf' and get the address.
    result = subprocess.check_output(
        f"{readelf} -s {file} --wide | grep \"" + symbol_name + "$\" | awk '{print $2}'",
        shell=True
    ).decode('utf-8')

    if result is not None and len(result.strip()) > 0:
        return result.strip()

    return None
