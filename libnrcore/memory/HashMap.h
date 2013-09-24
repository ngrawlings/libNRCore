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
        virtual ~HashMap() {}
        
        const char getKey() {
            return key;
        }
        
        void set(const char* map_key, T *obj) {
            HashMap *map;
            
            if (map_key[0]) {
                map = getHashMap(map_key[0]);
                if (!map)
                    this->map.push(new HashMap(map_key, obj));
                else
                    map->set(&map_key[1], obj);
            } else
                this->obj = obj;
                
        }
        
        T *get(const char* map_key) {
            HashMap *map;
            
            if (map_key[0]) {
                map = getHashMap(map_key[0]);
                if (map)
                    return map->get(&map_key[1]);
                else
                    return 0;
            } else
                return this->obj;
            
            return 0;
        }
        
    protected:
        HashMap(const char* map_key, T *obj) {
            obj = 0;
            key = map_key[0];
            
            if (key)
                map = new HashMap(&map_key[1], obj);
            else
                this->obj = obj;
        }
        
        HashMap *getHashMap(const char key) {
            int map_len = (int)map->length();
            for (int i=0; i<map_len; i++)
                if (map->operator[](i)->getKey() == key)
                    return map->operator[](i);
            return 0;
        }
        
    private:
        Array<HashMap*> *map;
        char    key;
        T       *obj;
    };
    
};

#endif /* defined(__libNRCore__HashMap__) */
