//
//  Serializable.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 19/05/2013.
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

#ifndef __nrcore__Serializable__
#define __nrcore__Serializable__

#include "LinkedList.h"
#include <libnrcore/debug/Log.h>

#include <libnrcore/memory/Ref.h>
#include <libnrcore/memory/ByteArray.h>
#include <libnrcore/memory/LinkedList.h>

namespace nrcore {

    class Serializable {
    public:
        enum OBJECT_TYPE {
            OBJECT_TYPE_INT8                =   0,
            OBJECT_TYPE_INT16               =   1,
            OBJECT_TYPE_INT32               =   2,
            OBJECT_TYPE_INT64               =   4,
            OBJECT_TYPE_TINY_BYTEARRAY      =   5,
            OBJECT_TYPE_MEDIUM_BYTEARRAY    =   6,
            OBJECT_TYPE_BYTEARRAY           =   7,
            OBJECT_TYPE_TINY_SERIALIZABLE   =   8,
            OBJECT_TYPE_MEDIUM_SERIALIZABLE =   9,
            OBJECT_TYPE_SERIALIZABLE        =   10,
            OBJECT_TYPE_OTHER               =   11
        };
        
        Serializable() {}
        virtual ~Serializable() {}
        
        ByteArray serialize();
        void unserialize(const Memory &bytes);
        
    protected:
        typedef struct _SERIAL_OBJECT {
            OBJECT_TYPE  type;
            size_t       len;
            void*        object;
        } SERIAL_OBJECT;
        
        virtual ByteArray serializeOther(int index, void* obj) { return ByteArray(); }
        virtual void beforeSerialization() {}
        virtual void serializedObjectLoaded(int index, SERIAL_OBJECT *so) {};
        
        void declareInt8(char *obj);
        void declareInt16(short *obj);
        void declareInt32(int *obj);
        void declareInt64(long long *obj);
        void declareByteArray(size_t len, char *obj);
        void declareSerializable(Serializable *obj);
        void declareOther(void *obj);
        
        void setObjectLength(int index, int len);
        
        void clearSerializationDeclarations();
        
    private:
        LinkedList< Ref<SERIAL_OBJECT> > serial_objects;
    };
    
};

#endif /* defined(__nrcore__Serializable__) */
