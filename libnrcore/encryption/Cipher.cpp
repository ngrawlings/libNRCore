//
//  Cipher.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 15/04/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "Cipher.h"

namespace nrcore {
  
    Memory Cipher::pad(const char* bytes, int len, int block_size) {
        char pval = block_size - len%block_size;
        int newlen = len + pval;
        
        char* input = new char[newlen];
        memcpy(input, bytes, len);
        
        for (int i = 0; i<pval; i++)
            input[len+i] = pval;
        
        return Memory(input, newlen);
    }
    
    int Cipher::unpaddedLength(const char* bytes, int len) {
        char pval = bytes[len-1];
        
        if ( pval <= 16 && pval > 0) {
            int i;
            for (i=0; i<pval; i++)
                if (bytes[len-1-i] != pval)
                    break;
            
            if (i == pval)
                len -= pval;
        } else
            return 0;
        
        return len;
    }
    
};