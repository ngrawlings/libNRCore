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
#include <libnrcore/debug/Log.h>

namespace nrcore {

    ByteArray Serializable::serialize() {
        ByteArray ret;
        int len = serial_objects.length();
        
        LinkedListState< Ref<SERIAL_OBJECT> > objs(&serial_objects);
        
        SERIAL_OBJECT *obj;
        ByteArray ba_obj;
        
        for (int i=0; i<len; i++) {
            ba_obj.clear();
            obj = objs.next().getPtr();
            
            ba_obj.append(ByteArray((const char*)&obj->type, 1));
            switch (obj->type) {
                case OBJECT_TYPE_OTHER:
                    {
                        ByteArray s = serializeOther(i, obj->object);
                        int len = s.length();
                        ba_obj.append(ByteArray((const char*)&len, 4));
                        ba_obj.append(s);
                    }
                    break;
                    
                case OBJECT_TYPE_SERIALIZABLE:
                    {
                        ByteArray s = reinterpret_cast<Serializable*>(obj->object)->serialize();
                        int len = s.length();
                        ba_obj.append(ByteArray((const char*)&len, 4));
                        ba_obj.append(s);
                    }
                    break;
                    
                default:
                    ba_obj.append(ByteArray((const char*)&obj->len, 4));
                    ba_obj.append(ByteArray((const char*)obj->object, obj->len));
            }
            
            ret.append(ba_obj);
        }
        
        return ret;
    }

    void Serializable::unserialize(ByteArray &bytes) {
        int len = bytes.length();
        const char* buf = bytes.operator char *();
        int offset = 0, cnt = 0;
        
        SERIAL_OBJECT so;
        
        while (offset < len) {
            so.type = (OBJECT_TYPE)buf[offset];
            memcpy(&so.len, &buf[offset+1], 4);
            so.object = (void*)&buf[offset+5];
            
            serializedObjectLoaded(cnt, &so);
            
            cnt++;
            offset += 5+so.len;
        }
    }

    void Serializable::declareInt8(char *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT8;
        so->len     = 1;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declareInt16(short *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT16;
        so->len     = 2;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declareInt32(int *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT32;
        so->len     = 4;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declareInt64(long long *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_INT64;
        so->len     = 8;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declareByteArray(size_t len, char *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_BYTEARRAY;
        so->len     = len;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }

    void Serializable::declareSerializable(SerializableInterface *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_SERIALIZABLE;
        so->len     = 0;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }
    
    void Serializable::declareOther(void *obj) {
        SERIAL_OBJECT *so = new SERIAL_OBJECT;
        so->type    = OBJECT_TYPE_OTHER;
        so->len     = 0;
        so->object  = obj;
        serial_objects.add( Ref<SERIAL_OBJECT>(so) );
    }
    
    void Serializable::setObjectLength(int index, int len) {
        SERIAL_OBJECT *so = serial_objects.get(index).getPtr();
        so->len = len;
    }

};
