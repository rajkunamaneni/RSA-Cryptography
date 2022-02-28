# Public Key Cryptography

This program is designed to send files securely to its intended client. This is done to keep the contents of the file not being viewable to prying eyes.This program makes sure that only the sender and receiver would see the contents of the file, as this program uses a private and public key. The private and public keys are generated with the file Keygen. With the keys created, we can encrypt and decrypt the file by following the help message as stated below to understand how to input and output a file.

## Build

Program build, run `make`.

Build a single program with:
 - '''
make keygen
'''
 - '''
make encrypt
'''
 - '''
make decrypt
'''

## Run

Run the program by creating the Public and Private keys via Keygen. View ./keygen -h to understand program functionality. Following keygen, run ./encrypt to encrypt any text provided and ./decrypt to decrypt the following encrypted file via the private key.

## Issues

The program currently has no documented issues.
