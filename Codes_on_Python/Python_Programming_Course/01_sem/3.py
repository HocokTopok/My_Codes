from functools import reduce

def print_denaty_sum(number: int):
  print(sum([x for x in range(1, number + 1)]) / 10)
  # print(reduce(lambda x, y: x+y, range(1, number+1)) / 10)

print_denaty_sum(2)