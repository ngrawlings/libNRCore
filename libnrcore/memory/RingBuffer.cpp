//
//  RingBuffer.cpp
//  libNRCore
//
//  Created by Nyhl on 03/11/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "RingBuffer.h"

#include </Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/usr/include/string.h>

namespace nrcore {

    RingBuffer::RingBuffer(size_t size) {
        buffer = new char[size];
        _size = size;
        read_cursor = write_cursor = 0;
        _length = 0;
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
    
    size_t RingBuffer::freeSpace() {
        return _size-_length;
    }

    size_t RingBuffer::append(const char *data, size_t len) {
        if (_size-2 < _length+len)
            len = _size - _length;
        
        if (!len)
            return 0;
        
        size_t space = _size-write_cursor;
        
        memcpy(&buffer[write_cursor], data, space >= len ? len : space);
        if (space < len)
            memcpy(buffer, &data[space], len-space);
        
        _length += len;
        write_cursor += len;
        write_cursor %= _size;
        
        return len;
    }
    
    RefArray<char> RingBuffer::fetch(size_t len) {
        if (len > _length)
            len = _length;
        
        char *ret = new char[len];
        
        size_t space = _size-read_cursor;
        
        memcpy(ret, &buffer[read_cursor], space >= len ? len : space);
        if (space < len)
            memcpy(&ret[space], buffer, len-space);
        
        _length -= len;
        read_cursor += len;
        read_cursor %= _size;
        
        return RefArray<char>(ret);
    }
    
    Memory RingBuffer::getDataUntilEnd() {
        if (read_cursor+_length >= _size)
            return Memory(&buffer[read_cursor], _size-read_cursor);
        else
            return Memory(&buffer[read_cursor], _length);
    }
    
    void RingBuffer::drop(int len) {
        if (len>0) {
            len = len < _length ? len : _length;
            read_cursor += len;
            read_cursor %= _size;
            _length -= len;
        }
    }

}
