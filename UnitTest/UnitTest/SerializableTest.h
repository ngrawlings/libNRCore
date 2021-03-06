//
//  SerializableTest.h
//  UnitTest
//
//  Created by Nyhl Rawlings on 9/25/13.
//  Copyright (c) 2013 Nyhl Rawlings. All rights reserved.
//

#ifndef __UnitTest__SerializableTest__
#define __UnitTest__SerializableTest__

#include <libnrcore/serializers/Serializable.h>
#include <libnrcore/memory/LinkedList.h>

using namespace nrcore;

class SerializableTest : public Serializable {
public:
    SerializableTest();
    virtual ~SerializableTest();
    

    int test1;
    int test2;
    char buf[32];
    LinkedList<int> list;
    
    
protected:
    
    void serializedObjectLoaded(int index, SERIAL_OBJECT *so);
    ByteArray serializeOther(int index, void* obj);
    
};

#endif /* defined(__UnitTest__SerializableTest__) */
