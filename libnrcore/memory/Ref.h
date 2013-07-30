//
//  Ref.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 02/11/2012.
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

#ifndef __PeerConnector__Ref__
#define __PeerConnector__Ref__

namespace nrcore {

    template <class T>
    class Ref {
    public:
        explicit Ref<T>(T* ptr, bool array=false){
            this->ptr = ptr;
            if (ptr) {
                cnt = new int;
                (*cnt) = 1;
            } else
                cnt = 0;
            this->array = array;
        }
        
        Ref<T>(const Ref<T>& ref) {
            ptr = ref.ptr;
            if (ref.ptr) {
                cnt = ref.cnt;
                *cnt = (*cnt)+1;
            } else
                cnt = 0;
            array = ref.array;
        }
        
        Ref<T>() {
            ptr = 0;
            cnt = 0;
            array = false;
        }
        
        virtual ~Ref () {
            decrement();
        }
        
        operator Ref<T>() {
            return Ref<T>(this);
        }
        
        operator T&() {
            return *ptr;
        }
        
        T &get() {
            return *ptr;
        }
        
        T *getPtr() {
            return ptr;
        }
        
        Ref<T>& operator= (const Ref<T>& ref) {
            decrement();
            
            ptr = ref.ptr;
            cnt = ref.cnt;
            array = ref.array;
            
            if (cnt)
                *cnt = (*cnt)+1;
            
            return *this;
        }
        
        void release() {
            ptr = 0;
            if (cnt)
                *cnt = 0;
        }
        
    protected:
        T *ptr;
        int *cnt;
        bool array;
        
    private:
        void decrement() {
            if (cnt) {
                (*cnt)--;
                if (*cnt == 0 && ptr != 0) {
                    if (array)
                        delete [] ptr;
                    else
                        delete ptr;
                    delete cnt;
                    ptr = 0;
                    cnt = 0;
                } else if(*cnt <= 0) {
                    delete cnt;
                    cnt = 0;
                }
            }
        }
    };
    
};

#endif /* defined(__PeerConnector__Ref__) */
