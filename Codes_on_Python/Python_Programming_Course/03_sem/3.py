import time
from functools import wraps

def amortized_timed_func(count):
    def inner(func):
        @wraps(func)
        def inner_inner(*args, **kwargs):
            sum_time = 0
            for _ in range(count):
                start = time.time()
                res = func(*args, **kwargs)
                sum_time += time.time() - start
            print(sum_time / count)
            return res
        return inner_inner
    return inner
    
@amortized_timed_func(5)
def say_hello():
    print("Hello")

class MyFunctor:
    def __call__(self):
        pass

a = MyFunctor()
a()

print(say_hello.__name__)