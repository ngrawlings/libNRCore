//
//  main.cpp
//  UnitTest
//
//  Created by Nyhl Rawlings on 9/21/13.
//  Copyright (c) 2013 Nyhl Rawlings. All rights reserved.
//

#include <libnrcore/memory/String.h>
#include <libnrcore/memory/ByteArray.h>

#include <iostream>

#include "SerializableTest.h"

using namespace nrcore;

int main(int argc, const char * argv[])
{

    // insert code here...
    String str = "test % test % test % test \r\n :) \r\n";
    str.arg(21).arg(22).arg(23);
    str.insert(0, "-> ");
    str.escape();
    printf("%s\r\n", str.operator char *());
    
    ByteArray ba("1234567890", 10);
    ba.append(ByteArray("12345 \x00 7890", 11));
    printf("%s\r\n", ba.toHex().operator char *());
    
    SerializableTest st;
    
    st.test1 = 55;
    st.test2 = 99;
    memcpy(st.buf, "1234567890", 11);
    
    ByteArray serial_bytes = st.serialize();
    
    printf("%s\r\n", serial_bytes.toHex().operator char *());
    
    SerializableTest new_st;
    new_st.unserialize(serial_bytes);
    
    ByteArray new_serial_bytes = new_st.serialize();
    printf("%s\r\n", new_serial_bytes.toHex().operator char *());
    
    if (serial_bytes == new_serial_bytes)
        printf("Serialization Working\r\n");
    else
        printf("Serialization Not Working\r\n");
    
    return 0;
}

