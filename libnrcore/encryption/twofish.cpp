#include "twofish.h"

namespace nrcore {

    TwoFish::TwoFish(const Memory &key, const Memory &iv)
    {
        setKey(key, iv);
    }

    void TwoFish::setKey(const Memory &key, const Memory &iv) {
        if (!cipherInit(&c_inst, MODE_CBC, iv.getMemory().getPtr()))
            return;

        makeKey(&k_encrypt, DIR_ENCRYPT, 256, key.getMemory().getPtr());
        makeKey(&k_decrypt, DIR_DECRYPT, 256, key.getMemory().getPtr());
    }

    CipherResult TwoFish::encrypt(const char* buf, int len) {
        if (len%(BLOCK_SIZE/8))
            throw -1;

        Memory input = pad(buf, len, getBlockSize());
        len = (int)input.length();
        
        char *result = new char[len];
        
        for (unsigned int i=0; i<len; i+=(BLOCK_SIZE/8))
            blockEncrypt(&c_inst, &k_encrypt, (BYTE*)&input.operator char *()[i], BLOCK_SIZE, (BYTE*)&result[i]);
        
        return CipherResult(result, len);
    }

    CipherResult TwoFish::decrypt(const char* buf, int len) {
        if (len%(BLOCK_SIZE/8))
            throw -1;

        char *result = new char[len];
        
        for (unsigned int i=0; i<len; i+=(BLOCK_SIZE/8))
            blockDecrypt(&c_inst, &k_decrypt, (BYTE*)&buf[i], (int)BLOCK_SIZE, (BYTE*)&result[i]);
        
        len = unpaddedLength(result, len);
        
        return CipherResult(result, len);
    }

};