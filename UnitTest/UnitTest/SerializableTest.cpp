//
//  SerializableTest.cpp
//  UnitTest
//
//  Created by Nyhl Rawlings on 9/25/13.
//  Copyright (c) 2013 Nyhl Rawlings. All rights reserved.
//

#include "SerializableTest.h"

SerializableTest::SerializableTest() {
    
    test1 = 0x11;
    test2 = 0x22;
    
    declareInt32(&test1);
    declareInt32(&test2);
    declareByteArray(32, buf);
    declareOther(&list);
    setObjectLength(2, 11);
    
    list.add(test1);
    list.add(test2);
}

SerializableTest::~SerializableTest() {
    
}

void SerializableTest::serializedObjectLoaded(int index, SERIAL_OBJECT *so) {
    switch (index) {
        case 0:
            memcpy(&test1, so->object, 4);
            break;
            
        case 1:
            memcpy(&test2, so->object, 4);
            break;
        
        case 2:
            memcpy(&buf, so->object, so->len);
            break;
            
        case 3:
            {
                list.clear();
                int count, offset = 4, val;
                const char *obj = (const char*)so->object;
                memcpy(&count, so->object, 4);
                for (int i=0; i<count; i++) {
                    memcpy(&val, &obj[offset], 4);
                    offset+=4;
                    list.add(val);
                }
            }
            break;
    }
}

ByteArray SerializableTest::serializeOther(int index, void* obj) {
    ByteArray ret;
    
    if (index == 3) {
        int len = list.length();
    
        ret.append(ByteArray(&len, 4));
    
        LinkedListState<int> state(&list);
        for (int i=0; i<len; i++) {
            int val = state.next();
            ret.append(ByteArray(&val, 4));
        }
    }
    
    return ret;
}