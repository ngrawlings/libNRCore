#ifndef TWOFISH_H
#define TWOFISH_H

#include "twofish/twofish2.h"

class TwoFish
{
public:
    TwoFish();

    bool setKey(char *key, char *iv);

    void encrypt(char const* in, char* result, size_t n);
    void decrypt(char const* in, char* result, size_t n);

private:
    cipherInstance c_inst;

    keyInstance k_encrypt, k_decrypt;
};

#endif // TWOFISH_H
