from copy import deepcopy

def with_copy_with(cls):
    def copy_with(self, *args, **kwargs):
        copy_obj = deepcopy(self)
        for key, value in kwargs.items():
            setattr(copy_obj, key, value)
        return copy_obj
    cls.copy_with = copy_with
    return cls

@with_copy_with
class Box:
    def __init__(self, w: int, h: int):
        self.w = w
        self.h = h

b = Box(1,2)
c = b.copy_with(h=10)
print(c.w, c.h)