#ifndef TWOFISH_H
#define TWOFISH_H

#include "Cipher.h"
#include "twofish/twofish2.h"

namespace nrcore {

    class TwoFish : Cipher
    {
    public:
        TwoFish(const Memory &key, const Memory &iv);

        void setKey(const Memory &key, const Memory &iv);

        virtual CipherResult encrypt(const char* buf, int len);
        virtual CipherResult decrypt(const char* buf, int len);
        
    private:
        cipherInstance c_inst;

        keyInstance k_encrypt, k_decrypt;
    };
    
};

#endif // TWOFISH_H
