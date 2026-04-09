from random import randint
from typing import Any

def count_calls(func):
    count = 0
    def inner(*args, **kwargs) -> Any:
        nonlocal count
        count += 1
        print(count)
        return func(*args, **kwargs)
    return inner

@count_calls
def get_random_int(low: int, high: int) -> int:
    return randint(low, high)


get_random_int(1, 10)
get_random_int(1, 10)
get_random_int(1, 10)
get_random_int(1, 10)