from typing import TypeVar, Type, Optional
from abc import ABC, abstractmethod

Runner = TypeVar("Runner", bound="BaseRunner")

_runners: dict[str, Type[Runner]] = {}


class BaseRunner(ABC):

    def __init__(self):
        pass

    @abstractmethod
    def start(self):
        raise NotImplementedError()

    @abstractmethod
    def restart(self):
        raise NotImplementedError()

    @staticmethod
    def register(name: str, runner: Type[Runner]):
        _runners[name] = runner

    @staticmethod
    def has_name(name: str) -> bool:
        return name in _runners

    @staticmethod
    def for_name(name: str, *args, **kwargs) -> Optional[Runner]:
        if name in _runners:
            return _runners[name](*args, **kwargs)
        else:
            return None
