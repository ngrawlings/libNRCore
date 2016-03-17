//
//  ByteArray.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 9/24/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__ByteArray__
#define __libNRCore__ByteArray__

#include "Ref.h"

#include "types.h"
#include <stdio.h>
#include <string.h>

#include "Memory.h"
#include "String.h"

namespace nrcore {
    
    class Memory;
    
    class ByteArray {
    public:
        ByteArray() : buffer(0), size(0), _length(0) {}
        ByteArray(const void *bytes, int len);
        ByteArray(const ByteArray &bytes);
        
        virtual ~ByteArray();
        
        ssize_t length() const {
        	return _length;
        }
        
        operator const char*() const {
            return buffer;
        }
        
        operator const Memory() {
            return Memory(buffer, _length);
        }
        
        ByteArray &operator =(const ByteArray &bytes) {
        	allocateBlock(bytes.length());
            _length = bytes.length();
            if (_length)
                memcpy(buffer, bytes.operator const char *(), _length);
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
        ByteArray &insert(int index, const char* ins, size_t len);
        ByteArray &insert(int index, ByteArray ins);
        ByteArray &replace(ByteArray search, ByteArray replace, int offset=0, int maxcnt=0);
        
        String toHex();
        static ByteArray fromHex(String hex);
        
    protected:
        char *buffer;
        size_t size, _length;
        
    private:
        void allocateBlock(size_t min_sz);
    };
    
};

#endif /* defined(__libNRCore__ByteArray__) */
