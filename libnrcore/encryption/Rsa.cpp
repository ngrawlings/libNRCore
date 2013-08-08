//
//  Rsa.cpp
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

#include "Rsa.h"

#include <string.h>

namespace nrcore {

    Rsa::Rsa(Memory& _cert, FORMAT cert_format, Memory& _key, FORMAT key_format) : privkey(0), pubkey(0), cert(0) {

        BIO *bio_key;
        Ref<char> pkey;

        initPublicCertificate(_cert, cert_format);

        if (_key.length()) {
            pkey = _key.getMemory();
            if (key_format == PEM) {
                bio_key = BIO_new_mem_buf(pkey.getPtr(), _key.length());
                privkey = PEM_read_bio_PrivateKey(bio_key, NULL, 0, NULL);
            } else {
                const unsigned char *tmp = (unsigned char*)((char*)pkey.getPtr());
                privkey = d2i_PrivateKey(EVP_PKEY_RSA, 0, &tmp, _cert.length());
            }
            dec = EVP_PKEY_get1_RSA(privkey);
            BIO_free(bio_key);
        }
    }

    Rsa::Rsa(Memory& _cert, FORMAT cert_format) : privkey(0), pubkey(0), cert(0) {
        initPublicCertificate(_cert, cert_format);
    }

    void Rsa::initPublicCertificate(Memory& _cert, FORMAT cert_format) {
        BIO *bio_cert;
        Ref<char> pcert;

        if (_cert.length()) {
            pcert = _cert.getMemory();
            if (cert_format == PEM) {
                bio_cert = BIO_new_mem_buf(pcert.getPtr(), _cert.length());
                cert = PEM_read_bio_X509(bio_cert, NULL, 0, NULL);
                BIO_free(bio_cert);
            } else {
                const unsigned char *tmp = (unsigned char*)((char*)pcert.getPtr());
                cert = d2i_X509(0, &tmp, _cert.length());
            }

            pubkey = X509_get_pubkey(cert);
            enc = EVP_PKEY_get1_RSA(pubkey);
        }
    }

    Rsa::~Rsa() {
        if (privkey) {
            EVP_PKEY_free(privkey);
            RSA_free(dec);
        }

        RSA_free(enc);
        EVP_PKEY_free(pubkey);
        X509_free(cert);
    }

    Ref<CipherResult> Rsa::encrypt(const char* buf, int len) {
        char* encrypt = new char[RSA_size(enc)];
        int encrypt_len = 0;

        if((encrypt_len = RSA_public_encrypt(len, (unsigned char*)buf, (unsigned char*)encrypt, enc, RSA_PKCS1_PADDING)) == -1) {
            Ref<CipherResult>(new CipherResult(0, -1));
        }

        return Ref<CipherResult>(new CipherResult(encrypt, encrypt_len));
    }

    Ref<CipherResult> Rsa::decrypt(const char*buf, int len) {
        char* decrypt = new char[RSA_size(dec)];
        int decrypt_len;
        if((decrypt_len = RSA_private_decrypt(len, (unsigned char*)buf, (unsigned char*)decrypt, dec, RSA_PKCS1_PADDING)) == -1) {
            Ref<CipherResult>(new CipherResult(0, -1));
        }

        return Ref<CipherResult>(new CipherResult(decrypt, decrypt_len));
    }

    Ref<Memory> Rsa::getCertificateBytes() {
        int len;
        unsigned char *buf = NULL;
        len = i2d_X509(cert, 0);

        if (len > 0) {
            unsigned char *tmp = buf = new unsigned char[len];
            i2d_X509(cert, &tmp);
            return Ref<Memory>(new Memory((char*)buf, len, true));
        }
        return Ref<Memory>(0);
    }
    
    bool Rsa::validate() {
        X509_STORE      *store      = NULL;
        X509_STORE_CTX  *vrfy_ctx   = NULL;
        
        if (!(store=X509_STORE_new()))
            return false;
        
        vrfy_ctx = X509_STORE_CTX_new();
        
        X509_STORE_CTX_init(vrfy_ctx, store, cert, NULL);
        
        return X509_verify_cert(vrfy_ctx) != 0;
    }
};
