//
//  Aes.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 06/11/2012.
//  Copyright (c) 2013. All rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// For affordable commercial licensing please contact nyhl@ngrawlings.com
//

//
// About This Class
//
// This class acts as a wrapper to the aes256cdc and conforms it to the Standard Cipher interface
//

#ifndef __PeerConnector__Aes__
#define __PeerConnector__Aes__

#include "Cipher.h"
#include <libnrcore/memory/Memory.h>

#define AES_BLOCK_SIZE 16

namespace nrcore {

    class Aes : public Cipher {
    public:
        enum BLOCK_CHAINING{ ECB=0, CBC=1, CFB=2 };
        
        Aes(const Memory &key, const Memory &iv);
        virtual ~Aes() {}
        
        void setKey(const Memory &key, const Memory &iv);
        
        CipherResult encrypt(const char* buf, int len);
        CipherResult decrypt(const char* buf, int len);
        
        void encryptBlock(const char* dat_in, char *dat_out);
        void decryptBlock(const char* dat_in, char *dat_out);
        
        int	getBlockSize();
        
        void reset();
        
    private:
        enum { DEFAULT_BLOCK_SIZE=16 };
        enum { MAX_BLOCK_SIZE=32, MAX_ROUNDS=14, MAX_KC=8, MAX_BC=8 };
        
        static const int sm_alog[256];
        static const int sm_log[256];
        static const char sm_S[256];
        static const char sm_Si[256];
        static const int sm_T1[256];
        static const int sm_T2[256];
        static const int sm_T3[256];
        static const int sm_T4[256];
        static const int sm_T5[256];
        static const int sm_T6[256];
        static const int sm_T7[256];
        static const int sm_T8[256];
        static const int sm_U1[256];
        static const int sm_U2[256];
        static const int sm_U3[256];
        static const int sm_U4[256];
        static const char sm_rcon[30];
        static const int sm_shifts[3][4][2];
        
        //Error Messages
        static char const* sm_szErrorMsg1;
        static char const* sm_szErrorMsg2;
        //Key Initialization Flag
        bool m_bKeyInit;
        //Encryption (m_Ke) round key
        int m_Ke[MAX_ROUNDS+1][MAX_BC];
        //Decryption (m_Kd) round key
        int m_Kd[MAX_ROUNDS+1][MAX_BC];
        //Key Length
        int m_keylength;
        //Block Size
        int	m_blockSize;
        //Number of Rounds
        int m_iROUNDS;
        //Chain Block
        char m_chain0[MAX_BLOCK_SIZE];
        char m_chain[MAX_BLOCK_SIZE];
        //Auxiliary private use buffers
        int tk[MAX_KC];
        int a[MAX_BC];
        int t[MAX_BC];
        
        static char const* sm_chain0;
        
        static int Mul(int a, int b);
        static int Mul4(int a, char b[]);
        
        bool Xor(char* buff, char const* chain);
        
        int GetKeyLength();
        int GetRounds();
        void ResetChain();
        
        bool MakeKey(char const* key, char const* chain, int keylength=DEFAULT_BLOCK_SIZE, int blockSize=DEFAULT_BLOCK_SIZE);
        bool DefEncryptBlock(char const* in, char* result);
        bool DefDecryptBlock(char const* in, char* result);
        bool EncryptBlock(char const* in, char* result);
        bool DecryptBlock(char const* in, char* result);
        
        Ref<char> Encrypt(const char *data_in, int *len);
        Ref<char> Decrypt(const char *data_in, int *len);
        
    };
    
};

#endif /* defined(__PeerConnector__Aes__) */
