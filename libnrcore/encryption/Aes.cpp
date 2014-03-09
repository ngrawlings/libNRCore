//
//  Aes.cpp
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

#include "Aes.h"
#include "Sha256.h"

namespace nrcore {

    Aes::Aes(const Memory& key, const Memory &iv) {
        setKey(key, iv);
    }

    void Aes::setKey(const Memory &key, const Memory &iv) {
        Sha256 sha;
        sha.update(key);
        sha.update(iv);
        sha.final();
        
        aes.MakeKey((const char*)sha.get(), iv.getBuffer(), 32, 16);
    }

    CipherResult Aes::encrypt(const char *buf, int len) {
        Ref<char> enc = aes.Encrypt(buf, &len);
        CipherResult result(enc.getPtr(), len);
        enc.release();
        return result;
    }

    CipherResult Aes::decrypt(const char *buf, int len) {
        Ref<char> dec = aes.Decrypt(buf, &len);
        CipherResult result(dec.getPtr(), len);
        dec.release();
        return result;
    }

    void Aes::encryptBlock(const char* dat_in, char *dat_out) {
        aes.EncryptBlock(dat_in, dat_out);
    }

    void Aes::decryptBlock(const char* dat_in, char *dat_out) {
        aes.DecryptBlock(dat_in, dat_out);
    }

};