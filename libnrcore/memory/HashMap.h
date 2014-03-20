//
//  HashMap.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 24/08/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__HashMap__
#define __libNRCore__HashMap__

#include "Array.h"

namespace nrcore {
    
    template <class T>
    class HashMap {
    public:
        HashMap() {}
        
        virtual ~HashMap() {
            clear();
        }
        
        const char getKey() {
            return key;
        }
        
        void set(const char* map_key, T newobj) {
            HashMap *hm;
            
            if (map_key[0]) {
                hm = getHashMap(map_key[0]);
                if (!hm)
                    map.push(new HashMap<T>(map_key, newobj));
                else
                    hm->set(&map_key[1], newobj);
            } else
                this->obj = newobj;
                
        }
        
        T get(const char* map_key) {
            HashMap *hm;
            
            if (map_key[0]) {
                hm = getHashMap(map_key[0]);
                if (hm)
                    return hm->get(&map_key[1]);
                else
                    return 0;
            } else
                return this->obj;
            
            return 0;
        }
        
        T *get(unsigned int index) {
            return map.get(index);
        }
        
        void clear() {
            int len;
            while ((len = (int)map.length())) {
                delete map.get(len-1);
                map.remove(len-1);
            }
        }
        
    protected:
        HashMap(const char* map_key, T newobj) {
            obj = 0;
            key = map_key[0];
            
            if (key && map_key[1]) {
                map.push(new HashMap<T>(&map_key[1], newobj));
            } else
                this->obj = newobj;
        }
        
        HashMap *getHashMap(const char key) {
            int map_len = (int)map.length();
            for (int i=0; i<map_len; i++) {
                HashMap<T>* hm = map.get(i);
                if (hm && hm->getKey() == key)
                    return map.get(i);
            }
            return 0;
        }
        
    private:
        Array<HashMap<T>*> map;
        char    key;
        T       obj;
    };
    
};

#endif /* defined(__libNRCore__HashMap__) */
