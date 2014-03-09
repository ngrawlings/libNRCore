//
//  Cipher.h
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

#ifndef PeerConnector_Cipher_h
#define PeerConnector_Cipher_h

#define ERROR_INVALID_KEY      1

namespace nrcore {

    class CipherResult {
    public:
        CipherResult(char *bytes, int length) {
            this->_bytes = bytes;
            this->_length = length;
        }
        
        CipherResult(const CipherResult &cipher) {
            this->_bytes = cipher.bytes();
            this->_length = cipher.length();
        }
        
        ~CipherResult() {
            if (_bytes)
                delete [] _bytes;
        }
        
        int length() const {
            return _length;
        }
        
        char* bytes() const {
            return _bytes;
        }
        
        void releaseWithoutFreeingMemory() {
            _bytes = 0;
        }
        
    private:
        char* _bytes;
        int _length;
    };

    class Cipher {
    public:
        virtual ~Cipher() {}

        virtual CipherResult encrypt(const char* buf, int len) = 0;
        virtual CipherResult decrypt(const char* buf, int len) = 0;
    };
    
};

#endif
