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

#include "Memory.h"
#include "String.h"

namespace nrcore {
    
    template <class T>
    class HashMap {
    public:
        HashMap() {
            parent = 0;
        }
        
        virtual ~HashMap() {
            clear();
        }
        
        const char getKey() {
            return key;
        }
        
        void set(Memory &map_key, T newobj) {
            HashMap<T> *hm = this, *pm = this;
            int i, key_len = (int)map_key.length();
            for (i=0; i<key_len; i++) {
                hm = hm->getHashMap(map_key[i]);
                if (!hm) {
                    hm = new HashMap<T>(pm, map_key[i], 0);
                    pm->map.push(hm);
                }
                pm = hm;
            }
            hm->setObject(newobj);
        }
        
        T get(Memory &map_key) {
            HashMap<T> *hm = this;
            int i, key_len = (int)map_key.length();
            for (i=0; i<key_len; i++) {
                hm = hm->getHashMap(map_key[i]);
                if (!hm)
                    break;
            }
            
            if (i==key_len)
                return hm->getObject();
            
            throw "Failed to find entry";
        }
        
        void remove(Memory &map_key) {
            HashMap<T> *hm = this, *cm;
            int i, key_len = (int)map_key.length();
            for (i=0; i<key_len; i++) {
                hm = hm->getHashMap(map_key[i]);
                if (!hm)
                    break;
            }
            
            if (i==key_len) {
                hm->setObject(0);
                
                while (!hm->getMapSize()) {
                    cm = hm;
                    hm = hm->getParent();
                    if (!hm) {
                        map.remove(cm);
                        break;
                    } else {
                        hm->removeMap(cm->getKey());
                        delete cm;
                    }
                }
            }
        }
        
        void clear() {
            int len;
            while ((len = (int)map.length())) {
                delete map.get(len-1);
                map.remove(len-1);
            }
        }
        
    protected:
        HashMap(HashMap<T> *parent, const char map_key, T newobj) {
            this->parent = parent;
            key = map_key;
            obj = newobj;
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
        
        void setObject(T obj) {
            this->obj = obj;
        }
        
        T getObject() {
            return obj;
        }
        
        void pushMap(const char* key, HashMap<T>* map) {
            this->map.push(map);
        }
        
        void removeMap(const char key) {
            int map_len = (int)map.length();
            for (int i=0; i<map_len; i++) {
                HashMap<T>* hm = map.get(i);
                if (hm && hm->getKey() == key)
                    map.remove(i);
            }
        }
        
        int getMapSize() {
            return (int)map.length();
        }
        
        HashMap<T> *getParent() {
            return parent;
        }
        
    private:
        HashMap<T> *parent;
        Array<HashMap<T>*> map;
        char    key;
        T       obj;
    };
    
};

#endif /* defined(__libNRCore__HashMap__) */
