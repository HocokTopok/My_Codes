def cached_result(func):
    memory = {}
    def inner(*args, **kwargs):
        key = ...
        if key in memory:
            return memory[key]
        
        res = func(*args, **kwargs)
        memory[key] = res
        return res
    return inner


@cached_result
def multiply_numbers(a, b):
    print(a * b)

multiply_numbers(2, 2)
multiply_numbers(3, 2)
multiply_numbers(2, 2)
multiply_numbers(2, 2)