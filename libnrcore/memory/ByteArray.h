//
//  ByteArray.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 9/24/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__ByteArray__
#define __libNRCore__ByteArray__

#include <libnrcore/base/Object.h>
#include "Ref.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "String.h"

namespace nrcore {
    
    class ByteArray : public Object {
    public:
        ByteArray() : buffer(0), size(0), _length(0) {}
        ByteArray(const void *bytes, int len);
        ByteArray(const ByteArray &bytes);
        
        virtual ~ByteArray();
        
        ssize_t length() {
        	return _length;
        }
        
        operator char*() {
            return buffer;
        }
        
        ByteArray &operator =(ByteArray bytes) {
        	_length = 0;
        	size_t len = bytes._length;
            if (len) {
                allocateBlock(len);
                _length = len;
                memcpy(buffer, bytes.buffer, len+1);
            }
        	return *this;
        }
        
        ByteArray &operator +=(ByteArray bytes) {
            append(bytes);
            return *this;
        }
        
        ByteArray operator +(ByteArray bytes) {
            ByteArray _s(*this);
            _s += bytes;
            return _s;
        }
        
        bool operator== (ByteArray &bytes)
        {
            return _length == bytes._length && !memcmp(buffer, bytes.buffer, _length);
        }
        
        bool operator!= (ByteArray &bytes)
        {
            return !(*this == bytes);
        }
        
        void clear();
        
        void append(const ByteArray &str);
        void append(void* bytes, int len);
        
        int indexOf(ByteArray search, int start=0);
        int occuranceCount(ByteArray search);
        
        ByteArray subBytes(int offset, int length=0);
        ByteArray &insert(int index, ByteArray ins);
        ByteArray &replace(ByteArray search, ByteArray replace, int offset=0, int maxcnt=0);
        
        String toHex();
        
    protected:
        char *buffer;
        size_t size, _length;
        
    private:
        void allocateBlock(size_t min_sz);
    };
    
};

#endif /* defined(__libNRCore__ByteArray__) */