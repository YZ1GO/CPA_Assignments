area = 0
N = 1000000
# for i in range(0, N-1):
#     x = (i + 0.5) / N
#     area += 1.0 / (1 + x ** 2)
# print (4 * area / N)

print (sum(map(lambda x: 1.0 / (1 + x ** 2), [(i + 0.5) / N for i in range(0, N-1)])) * 4 / N)