//
//  StaticArray.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 03/02/2013.
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

#ifndef PeerConnector_StaticArray_h
#define PeerConnector_StaticArray_h

namespace nrcore {

    template <class T, unsigned long sz>
    class StaticArray {
    public:
        StaticArray<T, sz>() {
            for (unsigned long i=0; i<sz; i++)
                array[i] = 0;
        }
        
        void set(unsigned long index, T obj) {
            if (index > sz)
                throw "Index to Big";
            
            array[index] = obj;
        }
        
        T& operator [](unsigned long index) {
            if (index > sz)
                throw "Index to Big";
            
            return array[index];
        }
        
    private:
        T array[sz];
    };
    
}

#endif
