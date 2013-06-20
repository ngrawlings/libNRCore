//
//  DescriptorInstanceMap.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 19/03/2013.
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

#ifndef __PeerConnectorCore__DescriptorInstanceMap__
#define __PeerConnectorCore__DescriptorInstanceMap__

#include <unistd.h>

namespace nrcore {

    template <class T>
    class DescriptorInstanceMap {
    public:
        typedef struct _DESCRIPTOR_INSTANCE_ENTRY {
            bool active;
            bool inuse;
            T instance;
        } DESCRIPTOR_INSTANCE_ENTRY;
        
    public:
        DescriptorInstanceMap<T>() : lock("Descriptor Table Lock") {
            max = (unsigned int)sysconf(_SC_OPEN_MAX);
            count = 0;
            decriptors = new DESCRIPTOR_INSTANCE_ENTRY[max];
            for (unsigned int i=0; i<max; i++) {
                decriptors[i].active    = false;
                decriptors[i].inuse     = false;
                decriptors[i].instance = 0;
            }
        }
        
        virtual ~DescriptorInstanceMap() {
            delete [] decriptors;
        }
        
        void set(unsigned long fd, T instance) {
            decriptors[fd].inuse = false;
            decriptors[fd].instance = instance;
        }
        
        bool equals(unsigned long fd, T instance) {
            return decriptors[fd].instance == instance;
        }
        
        T get(unsigned long fd) {
            if (decriptors[fd].active && !decriptors[fd].inuse)
                return decriptors[fd].instance;
            return 0;
        }
        
        T getAndLock(unsigned long fd) {
            if (decriptors[fd].active && !decriptors[fd].inuse) {
                decriptors[fd].inuse = true;
                return decriptors[fd].instance;
            }
            return 0;
        }
        
        void finished(unsigned long fd) {
            decriptors[fd].inuse = false;
        }
        
        void setActive(unsigned long fd, bool state) {
            if (!decriptors[fd].active && state)
                count++;
            else if (decriptors[fd].active && !state)
                count--;
                
            decriptors[fd].active = state;
        }
        
        bool isActive(unsigned long fd) {
            return decriptors[fd].active;
        }
        
        unsigned int getMaxDescriptors() {
            return max;
        }
        
        unsigned int getCount() {
            return count;
        }
        
        Mutex lock;
        
    private:
        unsigned int max;
        DESCRIPTOR_INSTANCE_ENTRY *decriptors;
        unsigned int count;
        
        
    };
    
}

#endif /* defined(__PeerConnectorCore__DescriptorInstanceMap__) */
