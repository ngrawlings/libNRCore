//
//  Memory.h
//  PeerConnector
//
//  Created by Nyhl Rawlings on 05/11/2012.
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

#ifndef __PeerConnector__Memory__
#define __PeerConnector__Memory__

#include <stdio.h>
#include <stdlib.h>

#include <libnrcore/base/Object.h>
#include "Ref.h"
#include <libnrcore/exception/Exception.h>

#include <string.h>
#include "String.h"

#define ERROR_OUT_OF_RANGE      1

namespace nrcore {

    class Memory : public Object {
    public:
        Memory() {
            buffer = Ref<char>(0);
            len = 0;
        }
        
        Memory(const void* buffer, size_t len) {
            this->buffer = Ref<char>(new char[len], true);
            this->len = len;
            memcpy(this->buffer.getPtr(), buffer, len);
        }
        
        Memory(const Memory &mem) {
            this->buffer = mem.buffer;
            this->len = mem.len;
        }
        
        virtual ~Memory() {
        }
        
        virtual Ref<char> getMemory() const {
            return buffer;
        }
        
        virtual operator char*() {
            return buffer.getPtr();
        }
        
        operator String() {
            return buffer.getPtr();
        }
        
        virtual char operator [](unsigned int index) {
            if (index<len)
                return ((char*)buffer.getPtr())[index];
            throw Exception(ERROR_OUT_OF_RANGE, (char*)"Index Out Of Range");
        }
        
        virtual size_t length() const {
            return len;
        }
        
        String toHex(bool uppercase) {
            String ret;
            char u, l;
            char * buf = buffer.getPtr();
            for (unsigned int i=0; i<len; i++) {
                u = (buf[i] & 0xF0) >> 4;
                l = buf[i] &0x0F;
                
                u += u < 10 ? 48 : uppercase ? 55 : 87;
                l += l < 10 ? 48 : uppercase ? 55 : 87;
                
                ret += u;
                ret += l;
            }
            
            return ret;
        }
        
        // Must free returned bytes with delete [] or set free_on_destory to true when initating Memory instance
        static char *getRandomBytes(int count) {
            char *ret = new char[count];
            while(count--)
                ret[count] = rand() % 256;
            return ret;
        }
        
    protected:
        Ref<char> buffer;
        size_t len;

    };
    
};

#endif /* defined(__PeerConnector__Memory__) */
