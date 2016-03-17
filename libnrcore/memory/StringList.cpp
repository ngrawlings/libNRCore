//
//  StringList.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 26/02/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "StringList.h"

#include "LinkedList.h"

namespace nrcore {

    StringList::StringList() {

    }

    StringList::StringList(String &str, String delimiter, int limit) {
        LinkedList<char*> parts;
        char *buf = new char[str.length()+1];
        memcpy(buf, str.operator char *(), str.length()+1);
        size_t dlen = strlen(delimiter);
        char *del;
        
        parts.add(buf);
        del = buf;
        while (parts.length() < limit || !limit) {
            del = strstr(del, delimiter);
            
            if (!del)
                break;
            
            del[0] = 0;
            del += dlen;
            
            parts.add(del);
        }
        
        LinkedListState<char*> pl(&parts);
        pl.first();
        
        while (pl.length()) {
            String *str = new String(pl.get());
            list.push(str);
            LINKEDLIST_NODE_HANDLE node = pl.getNode();
            pl.next();
            parts.remove(node);
        }
        
        delete [] buf;
    }

    StringList::StringList(const StringList &list) {
        this->list = list.list;
    }

    StringList::~StringList() {
    	int len = (int)list.length();
        for (int i=0; i<len; i++)
        	delete list[i];
    }

    size_t StringList::length() {
        return list.length();
    }
    
    void StringList::removeEmptyStrings() {
        int len = (int)list.length();
        
        for (int i=0; i<len; i++)
            if (!list[i]->length()) {
            	delete list[i];
                list.remove(i);
                i--;
                len--;
            }
    }
    
    void StringList::append(String str) {
        list.push(new String(str));
    }
    
};
