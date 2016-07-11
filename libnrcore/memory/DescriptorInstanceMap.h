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

#include <libnrcore/types.h>

namespace nrcore {

    template <class T>
    class DescriptorInstanceMap {
    public:
        DescriptorInstanceMap<T>() {
            max = (unsigned int)sysconf(_SC_OPEN_MAX);
            decriptors = new T[max];
            for (unsigned int i=0; i<max; i++)
                decriptors[i] = 0;
        }
        
        virtual ~DescriptorInstanceMap() {
            delete [] decriptors;
        }
        
        void set(unsigned long fd, T instance) {
            decriptors[fd] = instance;
        }
        
        bool equals(unsigned long fd, T instance) {
            return decriptors[fd] == instance;
        }
        
        T get(unsigned long fd) {
            return decriptors[fd];
        }
        
        unsigned int getMaxDescriptors() {
            return max;
        }

    private:
        unsigned int max;
        T *decriptors;
        
    };
    
}

#endif /* defined(__PeerConnectorCore__DescriptorInstanceMap__) */
