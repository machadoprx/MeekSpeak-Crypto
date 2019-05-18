from math import log, floor

def jacobi_calc(n, k):
	t = 0
	if n < 0: t = -1
	elif n == 0: return 0
	else: t = 1
	while n != 0:
		while n % 2 == 0:
			n = n >> 1
			r = k % 8
			if r == 3 or r == 5:
				t = -t
		n, k = k, n
		if n % 4 == k % 4 == 3:
			t = -t
		n = n % k
	if k == 1: return t
	return 0

def mod_inv(z, a):
	i, j, y_1, y_2 = a, z, 1, 0
	while j > 0:
		rem = i % j
		quo = i // j
		y = y_2 - (y_1 * quo)
		i, j, y_2, y_1 = j, rem, y_1, y
	return y_2 % a

def mont(x, m, R, beta):
	s1 = x & (R - 1)
	s2 = (beta * s1) & (R - 1)
	s3 = m * s2
	t = (x + s3) >> 256
	if t >= m:
		t = t - m
	return t

def mont_mul(x, y, R, p, beta):
	return mont(x * y, p, R, beta)

def mont_exp(x, e, p, beta, R):
	# R = pow(2, 32 * k)
	# shift left of k words
	x1 = (x * R) % p
	# A is precomputed
	A = R % p
	bin_str = bin(e)[2:]
	for i in range(len(bin_str)):
		A = mont_mul(A, A, R, p, beta)
		if bin_str[i] is '1':
			A = mont_mul(A, x1, R, p, beta)
	return mont_mul(A, 1, R, p, beta)

p25519 = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
b = 0x100000000
a = 0b11010111111110110111110111100110011111100110001111111110101110111110110110111110111001000111010111011111011011011111011100100011111100100011111111001000111111101101011101110010001111110101110111110110110111110111001000111111001000111111110000100011111110
c = 0b10101100110111101011011
'''k = floor(log(p25519, b)) + 1
R = pow(2, 32 * k)
beta = mod_inv(p25519, R)
beta *= -1'''
print(hex((pow(a, c, p25519))))
#print((pow(a, (p25519 - 1) // 2, p25519)))