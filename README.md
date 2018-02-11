# 42-Cryptography
Implementing cipher and message digest algorithms (like some of those found in openssl) as part of the 42 cryptography curriculum.

### 42 - Overview
The 42 curriculum limits the use of standard C library functions and imposes a style guideline called "The Norme".
This project has the following allowed libc functions:

* open
* close
* read
* write
* getpass (or readpassphrase or getch)
* malloc
* free

### Installation
To generate an executable for this project run `make` in the root of the project directory after you've downloaded or cloned it.
The make process will generate a binary called `ft_ssl`.

### Usage
After generating a binary, you can run the program with `./ft_ssl [command] [command args]`. 

Here is a list of supported commands for `ft_ssl` so far. Later projects in the 42 cryptography branch will add more commands to this list (which will be updated)!
```
Standard commands:

Message Digest commands:

Cipher commands:
base64
des       (alias for des-ecb)
des-ecb
des-cbc
des3      (alias for des3-cbc)
des3-ecb
des3-cbc

For more detailed usage use the -h option with specific commands!
./ft_ssl [command] -h
```

### Usage examples
Encrypting some plaintext to [base64](https://en.wikipedia.org/wiki/Base64):
```
> ./ft_ssl base64 -e <<< "I'm a plaintext string\!"
SSdtIGEgcGxhaW50ZXh0IHN0cmluZyEK
```
Decrypting base64 cipher text to plaintext:
```
> ./ft_ssl base64 -d <<< "SSdtIGEgcGxhaW50ZXh0IHN0cmluZyEK"
I'm a plaintext string!
```
Encrypting some plaintext with [DES](https://en.wikipedia.org/wiki/Data_Encryption_Standard)-[ECB](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_Codebook_(ECB)):
```
> ./ft_ssl des -k AABBCCDDEEFFAABB -a -e <<< "I'm going to be encrypted with des-ecb\!"
P8RwBKW16sMTM7L8dQlhDuoJXjY3psrpMPNblfwV9+Ij7vsLcetkBsSIDbTLGZQZ
```
Decrypting DES-ECB back to plaintext:
```
> ./ft_ssl des -k AABBCCDDEEFFAABB -a -d <<< "P8RwBKW16sMTM7L8dQlhDuoJXjY3psrpMPNblfwV9+Ij7vsLcetkBsSIDbTLGZQZ"
I'm going to be encrypted with des-ecb!
```
Encrypting some plaintext with [DES3](https://en.wikipedia.org/wiki/Triple_DES)-[CBC](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Cipher_Block_Chaining_(CBC)):
```
> ./ft_ssl des3 -k AABBCCDDEEFFAABB1122334455667788DDAABBCCDDAABBCC -v 8877665544332211 -a -e <<< "Triple the fun\!"
Z0tpVBnbqGE511s7PKx0V85r+LYlgSPm
```

Decrypting DES3-CBC back to plaintext:
```
> ./ft_ssl des3 -k AABBCCDDEEFFAABB1122334455667788DDAABBCCDDAABBCC -v 8877665544332211 -a -d <<< "Z0tpVBnbqGE511s7PKx0V85r+LYlgSPm"
Triple the fun!
```
### Useful resources
* https://www.di-mgt.com.au/cryptopad.html
* http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm
* https://en.wikipedia.org/wiki/Data_Encryption_Standard
* https://en.wikipedia.org/wiki/DES_supplementary_material#Rotations_in_the_key-schedule
* https://en.wikipedia.org/wiki/S-box
* https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
* https://en.wikipedia.org/wiki/Triple_DES

### Acknowledgements
Thanks to [Johnny](https://github.com/JohnnyJumper) and waliu @ 42 for battle testing my code!

Special thanks to [izcet](https://github.com/izcet) for ghostwriting the exercises for the 42 cryptography branch.
