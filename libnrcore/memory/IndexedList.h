//
//  IndexedList.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 07/11/2012.
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

#ifndef PeerConnector_IndexedList_h
#define PeerConnector_IndexedList_h

#include <libnrcore/base/Object.h>
#include <libnrcore/exception/Exception.h>
#include <libnrcore/threading/Thread.h>

namespace nrcore {

    template <class T>
    class IndexedListBase : public Object {
    public:
        IndexedListBase(int level, T ptr) {
            this->level = level-1;
            this->ptr = ptr;
        }
        virtual ~IndexedListBase() {}
        
    protected:
        T ptr;
        int level;
    };

    template <class T>
    class IndexedList : public IndexedListBase<T> {
    public:
        IndexedList<T> (int level) : IndexedListBase<T>(level, 0) {
            for (int i=0; i<256; i++)
                indexes[i] = 0;
            
            count = 0;
        }
        
        virtual ~IndexedList<T> () {
            for (int i=0; i<256; i++)
                if (indexes[i])
                    delete indexes[i];
        }
        
        T get(unsigned char *key) {
            if (!IndexedListBase<T>::level)
                return IndexedListBase<T>::ptr;
            else if (indexes[key[0]])
                return reinterpret_cast<IndexedList*>(indexes[key[0]])->get(&key[1]);
            
            throw Exception(-1, "No Such Entry");
        }
        
        void getNext(unsigned char *key) {
            if (count) {
                if (!IndexedListBase<T>::level)
                    incrementKey(key);
                
                for (int i=key[IndexedListBase<T>::level]; i<256; i++) {
                    if (!indexes[i])
                        incrementKey(key);
                }
            }
        }
        
        T set(unsigned char *key, T ptr) {
            T ret = 0;
            if (!ptr)
                return ptr;
            
            if (IndexedListBase<T>::level && !indexes[key[0]]) {
                if (IndexedListBase<T>::level == 1) {
                    indexes[key[0]] = new IndexedListBase<T>(IndexedListBase<T>::level, ptr);
                } else {
                    indexes[key[0]] = new IndexedList<T>(IndexedListBase<T>::level);
                    ret = reinterpret_cast<IndexedList*>(indexes[key[0]])->set(&key[1], ptr);
                }
            } else if (IndexedListBase<T>::level) {
                ret = reinterpret_cast<IndexedList*>(indexes[key[0]])->set(&key[1], ptr);
            } else {
                ret = this->ptr;
                this->ptr = ptr;
            }
            
            if (!ret)
                count++;
            
            return ret;
        }
        
        T remove(unsigned char *key) {
            T ret = this->ptr;
            if (IndexedListBase<T>::level && indexes[key[0]]) {
                ret = reinterpret_cast<IndexedList*>(indexes[key[0]])->remove(&key[1]);
                
                if (ret)
                    count--;
                
                if (!reinterpret_cast<IndexedList*>(indexes[key[0]])->getTreeLevelCount()) {
                    delete indexes[key[0]];
                    indexes[key[0]] = 0;
                }
            } else if (!IndexedListBase<T>::level)
                this->ptr = 0;
            
            return ret;
        }
        
        int getTreeLevelCount() {
            int ret = 0;
            for (int i=0; i<256; i++)
                if (IndexedListBase<T>::level && indexes[i])
                    ret++;
            return ret;
        }
        
        size_t getIndexedCount() {
            return count;
        }
        
    private:
        IndexedListBase<T>* indexes[256];
        size_t count;
        
        void incrementKey(unsigned char *key) {
            int l = IndexedListBase<T>::level;
            do {
                if (++key[l] != 0)
                    break;
            } while (l--);
        }
    };
    
};

#endif
