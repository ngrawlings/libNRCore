//
//  StringList.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 26/02/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__StringList__
#define __libNRCore__StringList__

#include "String.h"
#include "Array.h"

namespace nrcore {

    class StringList {
    public:
        StringList();
        StringList(String &str, String delimiter, int limit=0);
        StringList(const StringList &list);
        
        virtual ~StringList();
        
        String& operator [](unsigned int index) const {
            return *list.get(index);
        }
        
        size_t length() const {
            return list.length();
        }
        
        void removeEmptyStrings();
        void append(String str);
        
    protected:
        Array<String*> list;
    };
    
};

#endif /* defined(__libNRCore__StringList__) */
