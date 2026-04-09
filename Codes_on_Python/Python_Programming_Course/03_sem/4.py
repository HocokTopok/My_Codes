class Count:

    def __init__(self, func):
        self.func = func
        self.count = 0

    def __call__(self, *args, **kwargs):
        self.count += 1
        print(self.count)
        res = self.func(*args, **kwargs)
        return res
    
@Count
def say_hello(name):
    print(name)

say_hello("Goida")
say_hello("Goida")
say_hello("Goida")