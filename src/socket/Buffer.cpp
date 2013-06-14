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

namespace nrcore {

    Buffer::Buffer(int flags, int len) {
        _buffer = new Memory(new char[len], len, true);
        _fill = 0;
        _flags = flags;
    }

    Buffer::~Buffer() {
        delete _buffer;
    }

    Memory* Buffer::buffer() {
        return _buffer;
    }

    size_t  Buffer::fill() {
        return _fill;
    }

    size_t  Buffer::fill(size_t new_fill) {
        _fill = new_fill;
        return _fill;
    }

    void Buffer::incrementFill(size_t inc) {
        _fill += inc;
    }

    char    Buffer::flags() {
        return _flags;
    }

    char    Buffer::flags(char new_type) {
        _flags = new_type;
        return _flags;
    }

};