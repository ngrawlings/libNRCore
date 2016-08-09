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

#include <stdio.h>
#include <stdlib.h>

#include "Ref.h"

namespace nrcore {

    template <class T>
    class Array {
    public:
        Array<T>(int len=0) {
            this->len = len;
            _size = ((len / 16) * 16) + (len%16 ? 1 : 0);
            
            if (_size)
                array = new T[_size];
            else
                array = 0;
        }
        
        Array<T>(const Array<T> &a) {
            this->len = a.len;
            this->_size = a._size;
            
            if (_size) {
                array = new T[_size];
                for (int i=0; i<len; i++)
                    array[i] = a.array[i];
            } else {
                array = 0;
            }
        }
        
        virtual ~Array<T>() {
            if (array) {
                delete [] array;
                array = 0;
            }
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
            for (size_t i=index; i<len-1; i++)
                array[i] = array[i+1];
            len--;
        }
        
        void remove(T &obj) {
            int index = indexOf(obj);
            if (index != -1)
                remove(index);
        }
        
        T& get(unsigned int index) const {
            return array[index];
        }
        
        void push(const T& obj) {
            if (len == _size)
                grow();
            array[len++] = obj;
        }
        
        T pop() {
            return array[--len];
        }
        
        int indexOf(T& obj) {
            for (int i=0; i<len; i++)
                if (obj == array[i])
                    return i;
            return -1;
        }
        
        size_t length() const {
            return len;
        }
        
        size_t size() {
            return size;
        }
        
        T& operator [](unsigned int index) const {
            return array[index];
        }
        
    protected:
        size_t _size;
        size_t len;
        T *array;

        void grow() {
            T *tmp = new T[_size+16];
            
            for (size_t i=0; i<len; i++)
                tmp[i] = array[i];
            
            if (array)
                delete [] array;
            
            array = tmp;
            _size += 16;
        }
   
    };

};

#endif
