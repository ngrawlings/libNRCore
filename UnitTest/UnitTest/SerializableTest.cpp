//
//  SerializableTest.cpp
//  UnitTest
//
//  Created by Nyhl Rawlings on 9/25/13.
//  Copyright (c) 2013 Nyhl Rawlings. All rights reserved.
//

#include "SerializableTest.h"

SerializableTest::SerializableTest() {
    declareInt32(&test1);
    declareInt32(&test2);
    declareByteArray(32, buf);
    setObjectLength(2, 11);
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
    }
}