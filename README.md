# Public Key Cryptography

Public Key Cryptography Encryption and Decryption. The private and public keys are generated with the file Keygen. With the keys created, the program can encrypt and decrypt files.

## Required Library 

This program uses the GNU Multiple Precision Arithmetic Library (GMP). Download the [latest package](https://gmplib.org).

## Build

Program build, run `make`.

Build a single program with:

 - `make keygen`

 - `make encrypt`

 - `make decrypt`

## Run

Run the program by creating the Public and Private keys via Keygen. View ./keygen -h to understand program functionality. Following keygen, run ./encrypt to encrypt any text provided and ./decrypt to decrypt the following encrypted file via the private key.

## Issues

The program currently has no documented issues.
