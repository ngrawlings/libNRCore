//
//  Buffer.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 14/05/2013.
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

#include "Buffer.h"

#include "Socket.h"

namespace nrcore {

    Buffer::Buffer(Socket* sock) {
        this->sock = sock;
        len = 0;
    }

    Buffer::~Buffer() {
        mutex.lock();
        ByteArray *tmp;
        while (buffer.length()) {
            tmp = buffer.get(buffer.firstNode());
            buffer.remove(0);
            delete tmp;
        }
        mutex.release();
    }

    void Buffer::append(const char* bytes, int len) {
        mutex.lock();
        buffer.add(new ByteArray(bytes, len));
        this->len += len;
        mutex.release();
    }
    
    int Buffer::send() {
        int sent=0, res;
        ByteArray *mem;
        mutex.lock();
        LinkedListState<ByteArray*> b(&buffer);
        while (b.length() && b.next()) {
            mem = b.get();
            res = (int)::send(sock->getFd(), mem->operator const char *(), mem->length(), 0);
            
            if (res <= 0)
                break;
            
            sent += res;
            if (res == mem->length()) {
                b.remove();
                b.last();
                delete mem;
            } else {
                ByteArray tmp = mem->subBytes(res);
                mem->operator=(tmp);
                break;
            }
        }
        
        len -= sent;
        mutex.release();
        
        return sent;
    }
    
    int Buffer::length() {
        return len;
    }

};