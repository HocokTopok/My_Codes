from typing import Any, Iterable
from collections import Counter
from itertools import chain

def extract_collection(input_list: list[Any]) -> tuple[list[int], dict[str, int]]:
  
  # ints = list(filter(lambda x: type(x) is int, input_list))
  # counts = Counter(filter(lambda x: type(x) is str, input_list)).most_common()
  # counts = {some_key: value for some_key, value in counts}

  ints = []
  counts = {}
  
  for item in input_list:
    if type(item) is int:
      ints.append(item)
    elif type(item) is str:
      counts[item] = counts.get(item, 0) + 1
    elif isinstance(item, Iterable):
      nested_ints, nested_counts = extract_collection(item)
      ints.extend(nested_ints)
      for key, value in nested_counts.items():
        counts[key] = counts.get(key, 0) + value
    return ints, counts
