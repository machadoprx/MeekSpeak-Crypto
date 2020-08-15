# MeekSpeak-Crypto
Mini Crypto and big number suite with x25519 and AEAD_ChaCha20 (RFC7748 and RFC 7539).

Implemented features:

    * Elliptic Curve Point Multiplication as in RFC7748 (x25519 in constant time)
    * AEAD_ChaCha20 encryption as described in RFC7539
    * Fixed precision BigNumber library
    * Runs entirely on stack
    * No library dependency in Linux System

# How to use

Compile the code, after cloning the repository, with make:

```
make aead // for Elliptic Curve Diffie Hellman + AEAD_ChaCha20 Encrypt example

make bench // for benchmark and debug

make run
```

The functions uses are described in the main file, in benchmark folder and in the ecdh_aead_example.c
