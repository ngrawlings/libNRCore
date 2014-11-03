//
//  RingBuffer.h
//  libNRCore
//
//  Created by Nyhl on 03/11/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__RingBuffer__
#define __libNRCore__RingBuffer__

#include <libnrcore/types.h>
#include "RefArray.h"

namespace nrcore {

    class RingBuffer {
    public:
        RingBuffer(size_t size);
        virtual ~RingBuffer();
        
        size_t size();
        size_t length();
        
        void append(char *data, size_t len);
        RefArray<char> fetch(size_t len);
        
    private:
        size_t _size;    // Size of buffer
        size_t cursor;  // Start of data
        size_t _length;  // Amount of data in buffer
        
        char *buffer;
    };
    
}

#endif /* defined(__libNRCore__RingBuffer__) */
