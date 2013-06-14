//
//  Sha256.h
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

#ifndef PeerConnector_Sha256_h
#define PeerConnector_Sha256_h

#include "Hash.h"

#include <openssl/sha.h>

namespace nrcore {

    class Sha256 : public Hash {
    public:
        Sha256() {
            SHA256_Init(&ctx);
        }
        
        void update(const Memory& mem) {
            SHA256_Update(&ctx, ((Memory&)mem), ((Memory&)mem).length());
        }
        
        void final () {
            SHA256_Final(hash, &ctx);
        }
        
        size_t length() {
            return SHA256_DIGEST_LENGTH;
        }
        
        unsigned char* get() {
            return hash;
        }
        
        void reset() {
            SHA256_Init(&ctx);
        }
        
    private:
        SHA256_CTX ctx;
        unsigned char hash[SHA256_DIGEST_LENGTH];
    };

}

#endif
