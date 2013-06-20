//
//  ObjectBlockArray.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 23/11/2012.
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

#ifndef PeerConnector_ObjectBlockArray_h
#define PeerConnector_ObjectBlockArray_h

#include <core_config.h>

namespace nrcore {

    template <class T, int BLOCK_COUNT, int BLOCK_SIZE>
    class ObjectBlockArray {
    public:
        ObjectBlockArray() : count(0) {
            for (int i=0; i<BLOCK_COUNT; i++)
                object_blocks[i] = 0;
        }
        
        virtual ~ObjectBlockArray() {
            freeAllSlots();
        }
        
        T *allocateSlot(int *index) {
            for (int i=0; i<BLOCK_COUNT; i++) {
                if (!object_blocks[i]) {
                    object_blocks[i] = new OBJECT_BLOCK[BLOCK_SIZE];
                    object_blocks[i]->objects[0] = new T;
                    for (int x=1; x<BLOCK_SIZE; x++)
                        object_blocks[i]->objects[x] = 0;
                    
                    if (index)
                        *index = i*BLOCK_SIZE;
                    count++;
                    return object_blocks[i]->objects[0];
                }
                
                for (int x=0; x<BLOCK_SIZE; x++)
                    if (!object_blocks[i]->objects[x]) {
                        object_blocks[i]->objects[x] = new T;
                        count++;
                        if (index)
                            *index = (i*BLOCK_SIZE)+x;
                        return object_blocks[i]->objects[x];
                    }
            }
            return 0;
        }
        
        T setSlot(unsigned int slot, T obj) {
            int block = slot/BLOCK_SIZE;
            int index = slot%BLOCK_SIZE;
            
            if (!obj)
                return 0;
            
            if (!object_blocks[block]) {
                object_blocks[block] = new OBJECT_BLOCK[BLOCK_SIZE];
                
                for (int i=0; i<BLOCK_SIZE; i++)
                    object_blocks[block]->objects[i] = 0;
            } else if (!object_blocks[block]->objects[index])
                count++;
            
            object_blocks[block]->objects[index] = &obj;
            
            return *object_blocks[block]->objects[index];
        }
        
        T *get(unsigned int slot) {
            int block = slot/BLOCK_SIZE;
            if (object_blocks[block])
                return object_blocks[slot/BLOCK_SIZE]->objects[slot%BLOCK_SIZE];
            return 0;
        }
        
        void releaseSlot(int index, bool release_object=true) {
            if (!object_blocks[index/BLOCK_COUNT])
                return;
            
            count--;
            
            if (release_object)
                delete object_blocks[index/BLOCK_COUNT]->objects[index%BLOCK_SIZE];
            
            object_blocks[index/BLOCK_COUNT]->objects[index%BLOCK_SIZE] = 0;
            
            int cnt = 0;
            for (int x=0; x<BLOCK_SIZE; x++)
                if (object_blocks[index/BLOCK_COUNT]->objects[x])
                    cnt++;
            
            if (!cnt) {
                delete [] object_blocks[index/BLOCK_COUNT];
                object_blocks[index/BLOCK_COUNT] = 0;
            }
        }
        
        void freeAllSlots() {
            for (int i=0; i<BLOCK_COUNT; i++)
                if (object_blocks[i]) {
                    for (int x=0; x<BLOCK_SIZE; x++)
                        if (object_blocks[i]->objects[x])
                            delete object_blocks[i]->objects[x];
                    
                    delete [] object_blocks[i];
                }
            count = 0;
        }
        
        unsigned int length() {
            return count;
        }
        
    private:
        struct OBJECT_BLOCK {
            T *objects[BLOCK_COUNT];
        };
        OBJECT_BLOCK *object_blocks[BLOCK_SIZE];
        int count;
    };
    
};

#endif
