book_weights = [] # Код не доработан

while a := int(input()) != 0:
  book_weights.append(a)

def stats(data: list[int]) -> tuple[float, float]:
  m: float = sum(data) / len(data)
  sd = (sum([(x - m) ** 2 for x in data]) / len(data)) ** 0.5
  return m, sd

print(stats(book_weights))