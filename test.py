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


def b_mod_inv(a, p):
	if a < 1 or a >= p:
		return 0
	u, v, x_1, x_2 = a, p, 1, 0
	while u != 1 and v != 1:
		while u & 1 == 0:
			u = u >> 1
			if x_1 & 1 == 0:
				x_1 = x_1 >> 1
			else:
				x_1 = (x_1 + p) >> 1
		while v & 1 == 0:
			v = v >> 1
			if x_2 & 1 == 0:
				x_2 = x_2 >> 1
			else:
				x_2 = (x_2 + p) >> 1
		if u >= v: 
			u, x_1 = u - v, x_1 - x_2
		else:
			v, x_2 = v - u, x_2 - x_1
	if u == 1:
		return x_1 % p
	else:
		return x_2 % p

p = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
a = 0x35fedf799f98ffaefb6fb91d77db7dc8fc8ff23fb5dc8fd77db7dc8ff3fc23f9
b = 0b11101011001110100111100100101111011101110000110000001010010001100110001010001010111110001011101111100001001000001001101110101000100011000001100000011000101000001100001000101010010011111011001001000000010111000011100000001001100101010011110011001111011111
c = 0x9ac6241f
l = 0x1790f520c6645bdc6192b7da46c9382a5b9d8bf3e856a96e2c7018bc46f38534
d = 0x30591451fdebaf7c7c0457f47a3139c5db1bde9faa002f53134d7bb030ed3bbcebcd28b466227cc87766421df596a50c58c21d04c88ebf9ed887b58bf7112dc

print("MULT")
start = time()
r = a * b
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("SUB")
start = time()
r = b - a
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("SUM")
start = time()
r = a + b
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("POWERMOD")
start = time()
r = pow(a, c, p)
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("MODINV")
start = time()
r = mod_inv(a, p)
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("MOD")
start = time()
r = d % p
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("ONE RSHIFT")
start = time()
r = a >> 1
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("32 RSHIFT")
start = time()
r = a >> 32
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("AND")
start = time()
r = a & b
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("ONE LSHIFT")
start = time()
r = a << 1
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("32 LSHIFT")
start = time()
r = a << 32
end = time()
print(hex(r))
print("%0f\n" % (end - start))

print("LEGENDRE")
start = time()
r = pow(a, (p - 1) // 2, p)
end = time()
print(r)
print("%0f\n" % (end - start))