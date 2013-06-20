//
//  Rsa.h
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
// This class implments the standard Cipher interface for openssl RSA
//

#ifndef __RSA_H__
#define __RSA_H__

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>


#include <libnrcore/memory/Ref.h>
#include <libnrcore/memory/Memory.h>
#include "Cipher.h"

namespace nrcore {

    class Rsa : public Cipher {
    public:
        enum FORMAT {
            PEM,
            DER
        };
        
        Rsa(Memory& _cert, FORMAT cert_format, Memory& _key, FORMAT key_format);
        Rsa(Memory& _cert, FORMAT cert_format);
        virtual ~Rsa();
        
        Ref<CipherResult> encrypt(const char* buf, int len);
        Ref<CipherResult> decrypt(const char* buf, int len);
        
        Ref<Memory> getCertificateBytes();
        
    private:
        int block_size;
        EVP_PKEY *privkey, *pubkey;
        X509 *cert;
        RSA *enc, *dec;
        
        void initPublicCertificate(Memory& pem_cert, FORMAT cert_format);
    };
    
};

#endif//__RSA_H__
