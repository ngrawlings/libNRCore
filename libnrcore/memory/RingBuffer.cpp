//
//  RingBuffer.cpp
//  libNRCore
//
//  Created by Nyhl on 03/11/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "RingBuffer.h"

#include <string.h>

namespace nrcore {

    RingBuffer::RingBuffer(size_t size) {
        buffer = new char[size];
    }

    RingBuffer::~RingBuffer() {
        delete [] buffer;
    }

    size_t RingBuffer::size() {
        return _size;
    }

    size_t RingBuffer::length() {
        return _length;
    }

    void RingBuffer::append(char *data, size_t len) {
        if (_size < _length+len)
            len -= _size - _length;
        
        if (!len)
            return;
        
        size_t space = _size-cursor;
        
        memcpy(&buffer[cursor], data, space >= len ? len : space);
        if (space < len)
            memcpy(buffer, &data[space], len-space);
    }
    
    RefArray<char> RingBuffer::fetch(size_t len) {
        if (len > _length)
            len = _length;
        
        char *ret = new char[len];
        
        size_t space = _size-cursor;
        
        memcpy(ret, &buffer[cursor], space >= len ? len : space);
        if (space < len)
            memcpy(ret, buffer, len-space);
        
        cursor += len;
        cursor %= _size;
        
        return RefArray<char>(ret);
    }

}