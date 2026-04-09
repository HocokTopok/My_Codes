groom = {'A', 'B', 'C'}
bribe = {'B', 'C', 'D'}

print(len(groom | bribe))
print(sorted(list(groom.intersection(bribe))))
print(sorted(groom.symmetric_difference(bribe)))
print(sorted(bribe.difference(groom), reverse=True))