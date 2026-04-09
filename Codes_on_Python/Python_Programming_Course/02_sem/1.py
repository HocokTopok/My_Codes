from dataclasses import dataclass, field
from collections.abc import Callable
from typing import Any, TypeVar, Generic

T = TypeVar("T")

class Field(Generic[T]):
    def __init__(self, validator: Callable[[T], bool] | None = None, default: T = None, alias: str | None = None):
        self.validator = validator
        self.default = default
        self.alias = alias
        

    def __set_name__(self, owner, name):
        self.original_name = name


    def __get__(self, instance, owner):
        if instance is None:
            return self
        return instance.__dict__.get(self.original_name, self.default) or instance.__dict__.get(self.alias, self.default)


    def __set__(self, instance, value):
        if self.validator:
            if not self.validator(value):
                raise ValueError(f"Invalid value")
        

class Base:
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        for key, value in kwargs.items():
            setattr(self, key, value)

    
    def __repr__(self):
        if self.args:
            args_str = f"{', '.join(self.args)}"
        kwargs_str = ', '.join(f'{key} = {value}' for key, value in self.kwargs.items())
        return f"{self.__class__.__name__}({args_str}{kwargs_str})"


@dataclass(frozen = True, slots = True)
class AtrtistInDB(Base):
    id: int
    normalized_name: str
    reference_id: str
    canonical_id: str
    birth_date: str | None = Field[str, None](validator = ..., default = None, alias = "...")
    gender: str | None = None
    genres: list[str] | None = None

a = AtrtistInDB(1, 'Джони Деп', '0239487', birthDate = '01-01-1999')
print(a.__dict__)