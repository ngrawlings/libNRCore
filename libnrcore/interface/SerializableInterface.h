//
//  SerializableInterface.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 9/25/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__SerializableInterface__
#define __libNRCore__SerializableInterface__

#include <libnrcore/base/Object.h>
#include <libnrcore/memory/ByteArray.h>

namespace nrcore {
    
    class SerializableInterface : public Object {
    public:
        enum OBJECT_TYPE {
            OBJECT_TYPE_INT8            =   0,
            OBJECT_TYPE_INT16           =   1,
            OBJECT_TYPE_INT32           =   2,
            OBJECT_TYPE_INT64           =   4,
            OBJECT_TYPE_BYTEARRAY       =   5,
            OBJECT_TYPE_SERIALIZABLE    =   6,
            OBJECT_TYPE_OTHER           =   7
        };
        
        virtual ByteArray serialize() = 0;
        virtual void unserialize(const Memory &bytes) = 0;
        
    protected:
        virtual ByteArray serializeOther(int index, void* obj) { return ByteArray(); }
    };
    
}

#endif /* defined(__libNRCore__SerializableInterface__) */
