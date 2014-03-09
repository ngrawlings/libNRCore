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
#include <libnrcore/interface/SerializableInterface.h>
#include <libnrcore/debug/Log.h>

#include <libnrcore/memory/Ref.h>
#include <libnrcore/memory/ByteArray.h>
#include <libnrcore/memory/LinkedList.h>

namespace nrcore {

    class Serializable : public SerializableInterface {
    public:
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
        
        virtual void serializedObjectLoaded(int index, SERIAL_OBJECT *so) = 0;
        
        void declareInt8(char *obj);
        void declareInt16(short *obj);
        void declareInt32(int *obj);
        void declareInt64(long long *obj);
        void declareByteArray(size_t len, char *obj);
        void declareSerializable(SerializableInterface *obj);
        void declareOther(void *obj);
        
        void setObjectLength(int index, int len);
        
    private:
        LinkedList< Ref<SERIAL_OBJECT> > serial_objects;
    };
    
};

#endif /* defined(__PeerConnectorCore__Serializable__) */
