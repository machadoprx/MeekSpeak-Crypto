from math import log, floor

'''p = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
b = 0x100000000
k = floor(log(p, b)) + 1
u = floor((pow(b, 2 * k) / p))
bk_minus = pow(b, k - 1)

bk_plus = pow(b, k + 1)
print(bin(bk_plus - 1))
q2 = u / bk_plus
print(len(bin(bk_minus)) - 3)
print(len(bin(bk_plus)) - 3)'''
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


p224 = 0b11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001
p384 = 0b111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111
p25519 = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
a = 0b11000111011111111100100011111110100011111111110011001011111111001010010100100000011111110100011111111110011001011111111001010010100100011111111110011001111010001111111111001100101111111100101001010010010100100011001111111111
'''print(len(bin(a ** 2)))
print((a ** 2) > (p25519 ** 2))
c = 0b10101100110111101011011
print(hex(pow(a, c, p25519)))
b = 0x100000000
k = floor(log(p25519, b)) + 1
print(len(bin(pow(b, 2 * k))))'''
print(hex(pow(a, (p224 - 1) // 2, p224)))

