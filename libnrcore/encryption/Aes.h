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

#include "aes256cbc.h"
#include "Cipher.h"
#include <libnrcore/memory/Memory.h>

#define AES_BLOCK_SIZE 16

namespace nrcore {

    class Aes : public Cipher {
    public:
        Aes(const Memory &key, const Memory &iv);
        virtual ~Aes() {}
        
        void setKey(const Memory &key, const Memory &iv);
        
        CipherResult encrypt(const char* buf, int len);
        CipherResult decrypt(const char* buf, int len);
        
        void encryptBlock(const char* dat_in, char *dat_out);
        void decryptBlock(const char* dat_in, char *dat_out);
        
    private:
        AES256CBC aes;
        
        
    };
    
};

#endif /* defined(__PeerConnector__Aes__) */
