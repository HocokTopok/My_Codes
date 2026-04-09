from typing import Any

example = {
  "a": {
    "b": {
      "c": "+++",
    },
  },
}

def extractor(source: dict[str, Any], path: str, *, default: Any=None):
  list_path = path.split(".")
  temp = source
  for item in list_path:
    temp = temp.get(item, default)
    if temp == default:
      return default
    
  return temp
  

print(extractor(example, "a.b.c", default = None))