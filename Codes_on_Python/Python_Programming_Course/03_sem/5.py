from datetime import datetime

def call_logger(func):
    def inner(*args, **kwargs):
        res = func(*args, **kwargs)
        print(func.__name__, datetime.now(), list(args), dict(kwargs))
        return res
    return inner

@call_logger
def say_hi_1(name: str, need_upper: bool):
    print(f"Hi, {name.upper() if need_upper else name}")

@call_logger
def say_hi_2(name: str, need_upper: bool):
    print(f"Hi, {name.upper() if need_upper else name}")

say_hi_1("Goida", True)

say_hi_2("Goida", need_upper=True)

# def my_print(str1, str2, /, str3, *, str4, str5)