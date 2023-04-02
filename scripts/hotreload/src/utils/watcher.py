import time
from pathlib import Path
from typing import Callable, Optional

import watchdog.events
from watchdog.events import FileSystemEventHandler
from watchdog.observers.polling import PollingObserver

from .loaders import Loader, LoaderStyle


class Watcher:
    def __init__(self, paths: list[str], base: Path, on_change: Callable = None):
        self.paths = paths
        """The paths to watch for changes (recursively)"""

        self.base = base
        """The base directory that each of paths is relative to."""

        # Uses PollingObserver for best reliability (not noticed performance issues
        # even under WSL 2). Feel free to open a PR/issue if you think this won't work
        # for you.
        self.observer = PollingObserver(timeout=0.3)

        self.on_change = on_change
        """Callable to be invoked when change is detected."""

    def run(self):
        prepare_loader = Loader(LoaderStyle.INDETERMINATE_BAR).start("Tracking files...")

        handler = _WatcherChangeHandler(delegate=self.on_change)

        # Attempt to resolve each of the specified paths and, if they exist,
        # start watching them.
        for path in self.paths:
            try:
                resolved_path = self.base.joinpath(path).resolve(strict=True)
                if resolved_path.exists():
                    self.observer.schedule(
                        handler,
                        str(resolved_path),
                        recursive=True
                    )
            except Exception:
                pass

        self.observer.start()
        prepare_loader.stop()

        print()
        print("\033[1;92m(\u2714) Watching for changes...\033[0m")

        try:
            while True:
                time.sleep(0.6)
        finally:
            self.stop()

    def stop(self):
        self.observer.stop()
        self.observer.join()


class _WatcherChangeHandler(FileSystemEventHandler):

    def __init__(self, delegate: Optional[Callable] = None):
        self.delegate = delegate

    def on_any_event(self, event):
        # Do nothing when a directory is changed.
        if event.is_directory:
            return None

        if event.event_type in [
            watchdog.events.EVENT_TYPE_CREATED,
            watchdog.events.EVENT_TYPE_MODIFIED,
            watchdog.events.EVENT_TYPE_MOVED
        ]:
            # Then run delegate.
            print("\033[1;93m(!) Changes detected. Rebuilding...\033[0m")
            if self.delegate is not None:
                self.delegate()

