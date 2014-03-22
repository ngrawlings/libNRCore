//
//  SerializableString.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 22/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "SerializableString.h"

namespace nrcore {
    
    SerializableString::SerializableString(const char *str) : String(str) {}
    SerializableString::SerializableString(const String &str) : String(str) {}
    SerializableString::SerializableString(const char c) {}
    
    SerializableString::SerializableString(int num) : String(num) {}
    SerializableString::SerializableString(unsigned int num) : String(num) {}
    
    SerializableString::SerializableString(long num) : String(num) {}
    SerializableString::SerializableString(unsigned long num) : String(num) {}
    
    SerializableString::SerializableString(long long num) : String(num) {}
    SerializableString::SerializableString(unsigned long long num) : String(num) {}
    
    SerializableString::SerializableString(double num) : String(num) {}
    SerializableString::SerializableString(long double num) : String(num) {}
    
    SerializableString::~SerializableString() {}
    
    void SerializableString::beforeSerialization() {
        clearSerializationDeclarations();
        declareByteArray(_length, strbuf);
    }
    
    void SerializableString::serializedObjectLoaded(int index, SERIAL_OBJECT *so) {
        allocateBlock(so->len);
        _length = so->len;
        memcpy(strbuf, so->object, _length);
        strbuf[_length] = 0;
    }

};
