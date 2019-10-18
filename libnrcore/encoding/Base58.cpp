//
//  Base58.cpp
//  libnrcore
//
//  Created by Nyhl Rawlings on 17/10/2019.
//  Copyright © 2019 N G Rawlings. All rights reserved.
//

#include "Base58.h"
#include "impl/base58.h"

namespace nrcore {

    Memory Base58::encode(Memory mem) {
        size_t sz = ((mem.length() * 138) / 100) + 1;
        Memory ret(sz);
        
        if (b58enc(ret.operator char *(), &sz, mem.operator char *(), mem.length()))
            return ret;
        
        return Memory(0);
    }

    Memory Base58::decode(Memory mem) {
        size_t sz = ((mem.length() * 100) / 138);
        Memory ret(sz);
        
        if (b58tobin(ret.operator char *(), &sz, mem.operator char *(), mem.length()-1))
            return ret;
        
        return Memory(0);
    }

}
