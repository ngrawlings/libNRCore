//
//  Serializable.cpp
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

#include "Serializable.h"
#include <Debug/Log.h>

namespace nrcore {

    Ref<char> Serializable::serialize() {
        return Ref<char>(0);
    }

    Ref<Serializable> Serializable::unserialize() {
        return Ref<Serializable>(0);
    }

    void Serializable::declare_int8(char *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT8;
        so->len     = 1;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declare_int16(short *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT16;
        so->len     = 2;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declare_int32(int *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT32;
        so->len     = 4;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declare_int64(long long *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT64;
        so->len     = 8;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declare_bytearray(size_t len, char *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_BYTEARRAY;
        so->len     = len;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declare_serializable(Serializable *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_SERIALIZABLE;
        so->len     = 0;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    int Serializable::getAllocSize() {
        int ret = serial_objects.length();
        int cnt = ret;
        Ref<SERIAL_OBJECT> obj;
        
        LinkedListState< Ref<SERIAL_OBJECT> > objs(&serial_objects);
        
        // First cache all Serializable instance
        // Calculate length of non Serializable objects
        // Compile byte array
        
        obj = objs.first();
        while(cnt--) {
            
            
            
            obj = objs.next();
        }
        
        return 0;
    }

};