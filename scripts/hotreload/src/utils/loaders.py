import enum
import re
import sys
import itertools
import time
from io import StringIO, TextIOWrapper
from threading import Thread
from typing import Optional, Self, Callable


class SysioProxy(StringIO):
    pass


class LoaderStyle(enum.Enum):
    SPINNER = "spinner"
    INDETERMINATE_BAR = "indeterminate_bar"


class Loader:
    __styles: dict[LoaderStyle, any] = {
        LoaderStyle.SPINNER: itertools.cycle(['/', '-', '+', '\\', '|']),
        LoaderStyle.INDETERMINATE_BAR: itertools.cycle([
            '|---------|',
            '|>--------|',
            '|->-------|',
            '|-->------|',
            '|--->-----|',
            '|---->----|',
            '|----->---|',
            '|------>--|',
            '|------->-|',
            '|-------->|',
            '|---------|',
            '|--------<|',
            '|-------<-|',
            '|------<--|',
            '|-----<---|',
            '|----<----|',
            '|---<-----|',
            '|--<------|',
            '|-<-------|',
            '|<--------|',
        ])
    }

    def get_message(self) -> Optional[str]:
        return self._message

    def set_message(self, message: str) -> None:
        self._message = message
        self.__render_frame()

    message = property(get_message, set_message)

    def __init__(self,
                 style: LoaderStyle = LoaderStyle.SPINNER,
                 message: Optional[str] = None,
                 refresh_interval_ms=50,
                 show_timer: bool = True):
        if style not in self.__styles:
            raise KeyError(f"{style} is not a valid Loader style. Valid styles are: {self.__styles.keys()}")

        self.style = self.__styles[style]
        """The current loader style."""

        self.refresh_interval_ms = refresh_interval_ms
        """The amount of time to wait before showing the next frame."""

        self._message: Optional[str] = message
        """If specified, a message to be displayed alongside the loader."""

        self.show_timer = show_timer
        """Whether the loading 'timer' should be prepended to the loader message."""

        self.__thread: Optional[Thread] = None
        self.__frame: Optional[str] = None
        self.__start_time: Optional[int] = None

        self.__stdout_proxy = None
        self.__stderr_proxy = None

        self.__rendering = False
        """Mutex for spinning status."""

    def start(self, message: Optional[str] = None) -> Self:
        if self.__thread is not None:
            return

        if message is not None:
            self._message = message

        self.__start_time = int(time.time())

        # Install a standard output proxy if there isn't already one.
        self.__stdout_proxy = SysioProxy()
        sys.stdout = self.__stdout_proxy

        self.__stderr_proxy = SysioProxy()
        sys.stderr = self.__stderr_proxy

        self.__thread = Thread(target=self.__spin, daemon=True)
        self.__thread.start()

        return self

    def __render_frame(self):
        self.__rendering = True

        # Clear existing frame and return to start.
        if self.__frame is not None:
            self.__erase_line()
            sys.__stdout__.flush()

        # Check if the standard output proxy is waiting to print data.
        self.__flush_proxy(proxy=self.__stdout_proxy, stream=sys.__stdout__)
        self.__flush_proxy(proxy=self.__stderr_proxy, stream=sys.__stderr__)

        # Render new frame.
        self.__frame = next(self.style)
        self.__frame += (f' {self._message}' if self._message is not None else '')
        self.__frame += (f' ({int(time.time()) - self.__start_time}s)'
                                       if self.__start_time is not None and self.show_timer
                                       else '')
        self.__frame = f'\033[0;90m\033[1;97m{self.__frame}\033[0m'
        # Write the current frame, then jump to start of line.
        sys.__stdout__.write("\033[s")
        sys.__stdout__.write(self.__frame)
        sys.__stdout__.write("\033[u")
        # Flush output to prevent flickering.
        sys.__stdout__.flush()

        # Wait for next frame.
        self.__rendering = False

    def __spin(self):
        while self.__thread is not None:
            if self.__rendering:
                continue

            self.__render_frame()

            time.sleep(self.refresh_interval_ms / 1000)

    def stop(self) -> int:
        if self.__thread is None:
            return -1

        thread = self.__thread
        self.__thread = None
        thread.join()

        runtime = int(time.time()) - self.__start_time
        self.__start_time = None

        # Clear existing frame and return to start.
        sys.__stdout__.flush()

        # Restore standard output.
        if sys.stdout is not sys.__stdout__:
            sys.stdout = sys.__stdout__
        if sys.stderr is not sys.__stderr__:
            sys.stderr = sys.__stderr__

        if self.__frame is not None:
            self.__erase_line()
            sys.__stdout__.flush()

        # Flush any remaining proxied output
        self.__flush_proxy(proxy=self.__stdout_proxy, stream=sys.__stdout__)
        self.__flush_proxy(proxy=self.__stderr_proxy, stream=sys.__stderr__)

        # Return elapsed time (seconds).
        return runtime

    def get_proxy(self, stream):
        if stream == sys.__stdout__:
            return self.__stdout_proxy
        elif stream == sys.__stderr__:
            return self.__stderr_proxy
        return None

    @staticmethod
    def __flush_proxy(proxy: SysioProxy, stream: TextIOWrapper):
        if proxy is not None and proxy.tell() > 0:
            value = proxy.getvalue()

            proxy.seek(0)
            proxy.truncate(0)

            stream.write(value)
            stream.flush()

    def __erase_line(self):
        sys.__stdout__.write('\033[K')
        sys.__stdout__.write('\b' * self.__frame_length())
        sys.__stdout__.write(' ' * self.__frame_length())
        sys.__stdout__.write('\b' * self.__frame_length())

    def __frame_length(self) -> int:
        ansi_escape = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -/]*[@-~]')
        ansi_escapes_length = (len(self.__frame) - len(ansi_escape.sub('', self.__frame)))

        return max(len(self.__frame) - ansi_escapes_length, 0)


def load_while(execute: Callable, **kwargs):
    loader = Loader(**kwargs).start()
    result = execute()
    loader.stop()
    return result
