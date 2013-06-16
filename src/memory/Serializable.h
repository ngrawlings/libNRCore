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

#ifndef __PeerConnectorCore__Serializable__
#define __PeerConnectorCore__Serializable__

#include "LinkedList.h"
#include <libnrcore/Debug/Log.h>

#include <libnrcore/Memory/Ref.h>

namespace nrcore {

    class Serializable {
    public:
        Serializable() {}
        virtual ~Serializable() {}
        
        Ref<char> serialize();
        Ref<Serializable> unserialize();
        
    protected:
        enum OBJECT_TYPE {
            OBJECT_TYPE_INT8            =   0,
            OBJECT_TYPE_INT16           =   1,
            OBJECT_TYPE_INT32           =   2,
            OBJECT_TYPE_INT64           =   4,
            OBJECT_TYPE_BYTEARRAY       =   5,
            OBJECT_TYPE_SERIALIZABLE    =   6
        };
        
        virtual void declareSerializableObjects() = 0;
        
        void declare_int8(char *obj);
        void declare_int16(short *obj);
        void declare_int32(int *obj);
        void declare_int64(long long *obj);
        void declare_bytearray(size_t len, char *obj);
        void declare_serializable(Serializable *obj);
        
    private:
        typedef struct _SERIAL_OBJECT {
            OBJECT_TYPE  type;
            size_t       len;
            void*        object;
        } SERIAL_OBJECT;
        
        LinkedList< Ref<SERIAL_OBJECT> > serial_objects;
        
        int getAllocSize();
    };
    
};

#endif /* defined(__PeerConnectorCore__Serializable__) */
