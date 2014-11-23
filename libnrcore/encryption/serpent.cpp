/* serpent.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* serpent.c
 * a bitsliced implementation of the serpent cipher for avr microcontrollers
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 */

#include <stdint.h>
#include <string.h> /* memset() */
//#include <avr/pgmspace.h>
#include "serpent.h"

namespace nrcore {

    /******************************************************************************/

    const uint8_t sbox[] = {
     0x38, 0xF1, 0xA6, 0x5B, 0xED, 0x42, 0x70, 0x9C,
     0xFC, 0x27, 0x90, 0x5A, 0x1B, 0xE8, 0x6D, 0x34,
     0x86, 0x79, 0x3C, 0xAF, 0xD1, 0xE4, 0x0B, 0x52,
     0x0F, 0xB8, 0xC9, 0x63, 0xD1, 0x24, 0xA7, 0x5E,
     0x1F, 0x83, 0xC0, 0xB6, 0x25, 0x4A, 0x9E, 0x7D,
     0xF5, 0x2B, 0x4A, 0x9C, 0x03, 0xE8, 0xD6, 0x71,
     0x72, 0xC5, 0x84, 0x6B, 0xE9, 0x1F, 0xD3, 0xA0,
     0x1D, 0xF0, 0xE8, 0x2B, 0x74, 0xCA, 0x93, 0x56,
    /* now the inverted sboxes */
     0xD3, 0xB0, 0xA6, 0x5C, 0x1E, 0x47, 0xF9, 0x82,
     0x58, 0x2E, 0xF6, 0xC3, 0xB4, 0x79, 0x1D, 0xA0,
     0xC9, 0xF4, 0xBE, 0x12, 0x03, 0x6D, 0x58, 0xA7,
     0x09, 0xA7, 0xBE, 0x6D, 0x35, 0xC2, 0x48, 0xF1,
     0x50, 0x83, 0xA9, 0x7E, 0x2C, 0xB6, 0x4F, 0xD1,
     0x8F, 0x29, 0x41, 0xDE, 0xB6, 0x53, 0x7C, 0xA0,
     0xFA, 0x1D, 0x53, 0x60, 0x49, 0xE7, 0x2C, 0x8B,
     0x30, 0x6D, 0x9E, 0xF8, 0x5C, 0xB7, 0xA1, 0x42
    };


    #define SHR_O(a) c=(a)&1; ((a) = (a)>>1)
    #define SHR_I(a) ((a) = (c?0x80:0x00)| ((a)>>1))

    static void serpent_ip(uint32_t *i, uint8_t *o){
        uint8_t c; // carry
        uint8_t n,m;
        memset(o, 0, 16);
        for(n=0; n<16; ++n){
            for(m=0; m<2; ++m){
            SHR_O(i[0]);
            SHR_I(o[n]);
            SHR_O(i[1]);
            SHR_I(o[n]);
            SHR_O(i[2]);
            SHR_I(o[n]);
            SHR_O(i[3]);
            SHR_I(o[n]);
            }
        }
    }

    #undef SHR_I
    #define SHR_I(a) ((a) = (c?0x80000000L:0x00L)| ((a)>>1)) /* we use 32-bit words here */

    static void serpent_fp(uint32_t *i, uint32_t *o){
        uint8_t c; // carry
        uint8_t n,m;
        memset(o, 0, 16);
        for(n=0; n<4; ++n){
            for(m=0; m<8; ++m){
            SHR_O(i[n]);
            SHR_I(o[0]);
            SHR_O(i[n]);
            SHR_I(o[1]);
            SHR_O(i[n]);
            SHR_I(o[2]);
            SHR_O(i[n]);
            SHR_I(o[3]);
            }
        }
    }

    /******************************************************************************/
    static void sbox128x(uint8_t box, void* w){
        uint8_t sb[16];
        uint8_t i,t,x;
        box &= 0x0f;
        /* load sbox */
        for(i=0; i<8; ++i){
            t = *(sbox + box*8 + i);
            sb[2*i+0]=t>>4;
            sb[2*i+1]=t&0xf;
        }
        uint8_t o[16];
        serpent_ip((unsigned int*)w,o);

        for(i=0; i<16; ++i){
            t = ((uint8_t*)o)[i];
            x = sb[t>>4];
            x <<= 4;
            x |= sb[t&0xf];
            ((uint8_t*)o)[i] = x;
        }
        serpent_fp((uint32_t*)o, (uint32_t*)w);
    }

    void sbox128(void * w, uint8_t box){
        sbox128x(box&0x7, w);
    }

    void inv_sbox128(void * w, uint8_t box){
        sbox128x(((box&0x7)|0x8), w);
    }

    void* memxor (void *dest, const void *src, size_t n)
    {
        char const *s = (const char*)src;
        char *d = (char*)dest;

        for (; n > 0; n--)
            *d++ ^= *s++;

        return dest;
    }

    uint32_t rotl32(uint32_t a, uint8_t n){
        return ((a<<n) | (a>>(32-n)));
    }


    uint32_t rotr32(uint32_t a, uint8_t n){
        return ((a>>n) | (a<<(32-n)));
    }


    #define X0 (((uint32_t*)b)[0])
    #define X1 (((uint32_t*)b)[1])
    #define X2 (((uint32_t*)b)[2])
    #define X3 (((uint32_t*)b)[3])

    static void serpent_lt(uint8_t *b){
        X0 = rotl32(X0, 13);
        X2 = rotl32(X2,  3);
        X1 ^= X0 ^ X2;
        X3 ^= X2 ^ (X0 << 3);
        X1 = rotl32(X1, 1);
        X3 = rotl32(X3, 7);
        X0 ^= X1 ^ X3;
        X2 ^= X3 ^ (X1 << 7);
        X0 = rotl32(X0, 5);
        X2 = rotr32(X2, 10);
    }

    static void serpent_inv_lt(uint8_t *b){
        X2 = rotl32(X2, 10);
        X0 = rotr32(X0, 5);
        X2 ^= X3 ^ (X1 << 7);
        X0 ^= X1 ^ X3;
        X3 = rotr32(X3, 7);
        X1 = rotr32(X1, 1);
        X3 ^= X2 ^ (X0 << 3);
        X1 ^= X0 ^ X2;
        X2 = rotr32(X2,  3);
        X0 = rotr32(X0, 13);
    }

    #define GOLDEN_RATIO 0x9e3779b9l

    static uint32_t serpent_gen_w(uint32_t * b, uint8_t i){
        uint32_t ret;
        ret = b[0] ^ b[3] ^ b[5] ^ b[7] ^ GOLDEN_RATIO ^ (uint32_t)i;
        ret = rotl32(ret, 11);
        return ret;
    } 

    Serpent::Serpent(const Memory &key, const Memory &iv){
        setKey(key, iv);
    }
    
    void Serpent::setKey(const Memory &key, const Memory &iv) {
        uint32_t buffer[8];
        uint8_t i,j;
        int keysize_b = (int)key.length()+(int)iv.length();
        
        if(keysize_b<256){
            /* keysize is less than 256 bit, padding needed */
            char *_key = new char[keysize_b];
            memcpy(_key, key.getMemory().operator char *(), key.length());
            memcpy(&_key[key.length()], iv.getMemory().operator char *(), iv.length());
            
            memset(buffer, 0, 32);
            memcpy(buffer, _key, (keysize_b+7)/8);
            ((uint8_t*)buffer)[keysize_b/8] |= 1<<(keysize_b%8);
        } else {
            /* keysize is 256 bit */
            memcpy(buffer, key.getMemory().operator char *(), 32);
        }
        for(i=0; i<33; ++i){
            for(j=0; j<4; ++j){
                k[i][j] = serpent_gen_w(buffer, i*4+j);
                memmove(buffer, &(buffer[1]), 7*4); /* shift buffer one to the "left" */
                buffer[7] = k[i][j];
            }
        }
        for(i=0; i<33; ++i){
            sbox128(k[i],3-i);
        }
    }

    void Serpent::enc(void* buffer){
        uint8_t i;
        for(i=0; i<31; ++i){
            memxor(buffer, k[i], 16);
            sbox128(buffer, i);
            serpent_lt((uint8_t*)buffer);
        }
        memxor(buffer, k[i], 16);
        sbox128(buffer, i);
        ++i;
        memxor(buffer, k[i], 16);
    }

    void Serpent::dec(void* buffer){
        int8_t i=32;
        
        memxor(buffer, k[i], 16);
        --i;
        inv_sbox128(buffer, i);
        memxor((uint8_t*)buffer, k[i], 16);
        --i;
        for(; i>=0; --i){
            serpent_inv_lt((uint8_t*)buffer);
            inv_sbox128(buffer, i);
            memxor(buffer, k[i], 16);
        }
    }

    CipherResult Serpent::encrypt(const char* buf, int len)
    {
        if (len%16)
            throw "Invalid Length";
        
        Memory input = pad(buf, len, getBlockSize());
        char *result = new char[input.length()];
        memcpy(result, input.operator char *(), input.length());
        
        for (int i=0; i<len; i+=16)
            enc(&result[i]);
        
        return CipherResult(result, len);
    }

    CipherResult Serpent::decrypt(const char* buf, int len)
    {
        if (len%16)
            throw "Invalid Length";
        
        Memory input = pad(buf, len, getBlockSize());
        char *result = new char[input.length()];
        memcpy(result, input.operator char *(), input.length());
        
        for (int i=0; i<len; i+=16)
            dec(&result[i]);
        
        return CipherResult(result, len);
    }

};
