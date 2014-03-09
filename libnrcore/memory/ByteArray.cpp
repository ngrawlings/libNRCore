//
//  ByteArray.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 9/24/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#include "ByteArray.h"

#include <string.h>
#include "LinkedList.h"
#include "Array.h"

#define GET_HEX_HIGH_CHAR(x) ((x>>4)&0x0F)<10 ? ((x>>4)&0x0F)+48 : ((x>>4)&0x0F)+55;
#define GET_HEX_LOW_CHAR(x) (x&0x0F)<10 ? (x&0x0F)+48 : (x&0x0F)+55;

namespace nrcore {
    
    ByteArray::ByteArray(const void *bytes, int len) : buffer(0), _length(0) {
        allocateBlock(len);
        _length = len;
        memcpy(buffer, bytes, _length);
    }
    
    ByteArray::ByteArray(const ByteArray &bytes) : buffer(0), _length(0) {
        allocateBlock(bytes._length);
        _length = bytes._length;
        memcpy(buffer, bytes.buffer, _length);
    }
    
    ByteArray::~ByteArray() {
        if (buffer)
            delete [] buffer;
    }
    
    void ByteArray::allocateBlock(size_t min_sz) {
        size_t mask = -16;
        size = ((min_sz)&mask)+16;
        char *block = new char[size];
        
        if (_length && buffer != 0)
            memcpy(block, buffer, _length);
        
        if (buffer)
            delete [] buffer;
        
        buffer = block;
    }
    
    void ByteArray::clear() {
        _length = 0;
    }
    
    void ByteArray::append(const ByteArray &bytes) {
        if (size <= _length+bytes._length)
            allocateBlock(_length+bytes._length);
        memcpy(&buffer[_length], bytes.buffer, bytes._length);
        _length += bytes._length;
    }
    
    void ByteArray::append(void* bytes, int len) {
        if (size <= _length+len)
            allocateBlock(_length+len);
        memcpy(&buffer[_length], bytes, len);
        _length += len;
    }
    
    int ByteArray::indexOf(ByteArray search, int start) {
        bool found;
        ssize_t slen = search.length();
        
        for (unsigned int i=start; i<=_length-slen; i++) {
            found = true;
            for (int x=0; x<slen; x++) {
                found = buffer[i+x] == search.buffer[x];
                if (!found)
                    break;
            }
            if (found)
                return i;
        }
        return -1;
    }
    
    int ByteArray::occuranceCount(ByteArray search) {
        int offset = 0;
        int cnt = 0;
        
        while ((offset = indexOf(search, offset)) != -1) {
            cnt++;
            offset++;
        }
        
        return cnt;
    }
    
    ByteArray ByteArray::subBytes(int offset, int length) {
        length = length ? length : (int)_length-offset;
        
        char *buf = new char[length];
        
        memcpy(buf, &buffer[offset], length);
        buf[length] = 0;
        
        ByteArray ret(buf, length);
        delete buf;
        
        return ret;
    }
    
    ByteArray &ByteArray::insert(int index, ByteArray ins) {
        ssize_t ins_len = ins.length();
        
        if (_length+ins_len >= size)
            allocateBlock(_length+ins_len);
        
        for (ssize_t i=_length; i>=index; i--)
            buffer[i+ins_len] = buffer[i];
        
        memcpy(&buffer[index], ins.operator const char *(), ins_len);
        
        _length += ins_len;
        
        return *this;
    }
    
    ByteArray &ByteArray::replace(ByteArray search, ByteArray replace, int offset, int maxcnt) {
        ssize_t cnt=0, index, size_dif, slen, rlen;
        
        slen = search.length();
        rlen = replace.length();
        size_dif = rlen - slen;
        
        while ((index = indexOf(search, offset)) != -1) {
            if (maxcnt > 0 && cnt == maxcnt)
                break;
            
            if (size_dif > 0) {
                if (_length+size_dif >= size)
                    allocateBlock(_length+size_dif);
                
                for (ssize_t i=_length; i>=index+slen; i--)
                    buffer[i+size_dif] = buffer[i];
            } else if (size_dif < 0) {
                for (ssize_t i=index+rlen; i<_length; i++)
                    buffer[i] = buffer[i-size_dif];
            }
            
            memcpy(&buffer[index], replace.operator const char *(), rlen);
            
            _length += size_dif;
            
            cnt++;
        }
        
        return *this;
    }
    
    String ByteArray::toHex() {
        char tmp[3];
        String ret;
        
        tmp[2] = 0;
        
        for (unsigned int i=0; i<_length; i++) {
            tmp[0] = GET_HEX_HIGH_CHAR(buffer[i]);
            tmp[1] = GET_HEX_LOW_CHAR(buffer[i]);
            ret += tmp;
        }
        
        return ret;
    }
    
};
