# Assignment 6: Public Key Cryptography 
This is a program that encrypts and decrypts messages. This is possible through a mixture of public 
key and symmetric key cryptography.
The keygen program will be in charge of key generation, producing RSA public and private key pairs.
The encrypt program will encrypt files using a public key, and the decrypt program will decrypt the
encrypted files using the corresponding private key

## Build
    
    $ make

## Running

### keygen

For the encode program,you can specify the following command line arguments.

- `-b`: specifies the minimum bits needed for the public modulus nShows guidance for program command line
- `-i`: specifies the number of Miller-Rabin iterations for testing primes (default: 50)
- `-n`: specifies the public key file (default: rsa.pub).
- `-d`: specifies the private key file (default: rsa.priv).
- `-s`: specifies the random seed for the random state initialization
- `-v`: enables verbose output.
- `-h`: displays program synopsis and usage.

### encrypt
Unfortunatly, even though the code seems to be correct, it does not return anything. 
For the encrypt program, you can specify the following command line arguments.

- `-i`: specifies the input file to encrypt (default: stdin).
- `-o`: specifies the output file to encrypt (default: stdout).
- `-n`: specifies the file containing the public key (default: rsa.pub).
- `-v`: enables verbose output.
- `-h`: displays program synopsis and usage.

### decrypt
Unfortunatly, even though the code seems to be correct, it does not return anything.
For the decypt program,you can specify the following command line arguments.

- `-i`: specifies the input file to decrypt (default: stdin).
- `-o`: specifies the output file to decrypt (default: stdout).
- `-n`: specifies the file containing the private key (default: rsa.priv).
- `-v`: enables verbose output.
- `-h`: displays program synopsis and usage.
