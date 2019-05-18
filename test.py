from math import log, floor
from random import getrandbits
from time import time

def mod_inv(z, a):
	i, j, y_1, y_2 = a, z, 1, 0
	while j > 0:
		rem = i % j
		quo = i // j
		y = y_2 - (y_1 * quo)
		i, j, y_2, y_1 = j, rem, y_1, y
	return y_2 % a

p = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
a = 0b11010111111110110111110111100110011111100110001111111110101110111110110110111110111001000111010111011111011011011111011100100011111100100011111111001000111111101101011101110010001111110101110111110110110111110111001000111111110011111111000010001111111001
b = 0b11101011001110100111100100101111011101110000110000001010010001100110001010001010111110001011101111100001001000001001101110101000100011000001100000011000101000001100001000101010010011111011001001000000010111000011100000001001100101010011110011001111011111
c = 0b1001011100111100101

start = time()
print(hex(a * b))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(b - a))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(a + b))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(pow(a, c, p)))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(mod_inv(a, p)))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(a % p))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(a >> 1))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(a >> 32))
end = time()
print("%0f" % (end - start))

start = time()
print(hex(a & b))
end = time()
print("%0f" % (end - start))