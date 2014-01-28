//
//  Array.h
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

#ifndef PeerConnectorCore_Array_h
#define PeerConnectorCore_Array_h

#include <libnrcore/base/Object.h>

namespace nrcore {

    template <class T>
    class Array : public Object {
    public:
        Array<T>(int len) : auto_release(false) {
            this->len = len;
            _size = ((len / 16) * 16) + (len%16 ? 1 : 0);
            array = new T[_size];
        }
        
        virtual ~Array<T>() {
            if (auto_release)
                for (size_t i=0; i<len; i++)
                    if (array[i])
                        delete array[i];
            
            delete [] array;
        }
        
        T& operator [](unsigned int index) {
            return array[index];
        }
        
        void insert(int index, T& obj) {
            if (len == _size)
                grow();
            
            for (int i=len+1; i>index; i--)
                array[i] = array[i-1];
            len++;
            
            array[index] = obj;
        }
        
        void remove(int index) {
            for (size_t i=len; i>index; i--)
                array[i] = array[i-1];
            len--;
        }
        
        void push(const T& obj) {
            if (len == _size)
                grow();
            array[len++] = obj;
        }
        
        Ref<T> pop() {
            return Ref<T>(array[--len]);
        }
        
        void autoRelease(bool op) {
            auto_release = op;
        }
        
        size_t length() {
            return len;
        }
        
        size_t size() {
            return size;
        }
        
    protected:
        void grow() {
            T *tmp = new T[_size+16];
            
            for (size_t i=0; i<len; i++)
                tmp[i] = array[i];
            
            delete [] array;
            array = tmp;
            _size += 16;
            
            for (size_t i=len+1; i<_size; i++)
                array[i] = 0;
        }
        
    private:
        size_t _size;
        size_t len;
        T *array;
        
        bool auto_release;
    };

};

#endif
