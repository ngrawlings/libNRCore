#include "twofish.h"

TwoFish::TwoFish()
{
}

bool TwoFish::setKey(char *key, char *iv) {
    if (!cipherInit(&c_inst, MODE_CBC, iv))
        return false;

    makeKey(&k_encrypt, DIR_ENCRYPT, 256, key);
    makeKey(&k_decrypt, DIR_DECRYPT, 256, key);

    return true;
}

void TwoFish::encrypt(char const* in, char* result, size_t n) {
    if (n%(BLOCK_SIZE/8))
        throw "Input not a multiple of blocksize";

    for (unsigned int i=0; i<n; i+=(BLOCK_SIZE/8))
        blockEncrypt(&c_inst, &k_encrypt, (BYTE*)&in[i], n, (BYTE*)&result[i]);
}

void TwoFish::decrypt(char const* in, char* result, size_t n) {
    if (n%(BLOCK_SIZE/8))
        throw "Input not a multiple of blocksize";

    for (unsigned int i=0; i<n; i+=(BLOCK_SIZE/8))
        blockDecrypt(&c_inst, &k_decrypt, (BYTE*)&in[i], n, (BYTE*)&result[i]);
}
