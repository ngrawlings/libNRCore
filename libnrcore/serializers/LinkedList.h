//
//  LinkedList.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 9/26/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef libNRCore_Serializer_LinkedList_h
#define libNRCore_Serializer_LinkedList_h

#include <libnrcore/memory/LinkedList.h>
#include <libnrcore/memory/ByteArray.h>

namespace nrcore {
    
    namespace serializer {
    
        template <class T>
        class LinkedListSerializable {
        public:
            static ByteArray serialize(LinkedList<T> &list) {
                ByteArray ret;
                int obj_sz;
                int count = list.length();
                
                ret.append(ByteArray(&count, 4));
                if (count) {
                    LINKEDLIST_NODE_HANDLE node = list.prevNode(list.firstNode());
                    for (int i=0; i<count; i++) {
                        node = list.nextNode(node);
                        
                        ByteArray obj = list.get(node).serialize();
                        obj_sz = (int)obj.length();
                        ret.append(&obj_sz, 4);
                        ret.append(obj);
                    }
                }
                
                return ret;
            }
            
            static LinkedList<T> unserialize(ByteArray bytes) {
                LinkedList<T> ret;
                T obj;
                const char *buf = bytes;
                int cnt, len, offset = 4;
                memcpy(&cnt, buf, 4);
                
                for (int i=0; i<cnt; i++) {
                    memcpy(&len, &buf[offset], 4);
                    ByteArray ba(&buf[offset+4], len);
                    obj.unserialize(ba);
                    
                    ret.add(obj);
                    
                    offset += 4+len;
                }
                
                return ret;
            }
        };
        
        template <class T>
        class LinkedListIntegral {
        public:
            static ByteArray serialize(LinkedList<T> &list) {
                ByteArray ret;
                int count = list.length();
                
                ret.append(ByteArray(&count, 4));
                if (count) {
                    LINKEDLIST_NODE_HANDLE node = list.prevNode(list.firstNode());
                    for (int i=0; i<count; i++) {
                        node = list.nextNode(node);
                        ret.append(&list.get(node), sizeof(T));
                    }
                }
                
                return ret;
            }
            
            static LinkedList<T> unserialize(ByteArray bytes) {
                LinkedList<T> ret;
                const char *buf = bytes;
                int cnt, offset = 4;
                memcpy(&cnt, buf, 4);
                
                for (int i=0; i<cnt; i++) {
                    ret.add(*((int*)(&buf[offset])));
                    offset += sizeof(T);
                }
                
                return ret;
            }
        };
        
    }
    
}

#endif
