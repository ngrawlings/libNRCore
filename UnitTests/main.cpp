//
//  main.cpp
//  UnitTests
//
//  Created by Nyhl Rawlings on 03/06/2019.
//  Copyright © 2019 N G Rawlings. All rights reserved.
//

#include <iostream>
#include <libnrdebug/UnitTests.h>

#include <libnrcore/memory/ByteArray.h>

using namespace nrcore;

bool testByteArray() {
    ByteArray ba((char[]){0x01, 0x01, 0x01, 0x01}, 4);
    
    ba.shift(2);
    ba = ba.subBytes(1);
    
    ByteArray ba_s1((char[]){static_cast<char>(0x40), static_cast<char>(0x40), static_cast<char>(0x40), static_cast<char>(0x40)}, 4);
    
    if (ba != ba_s1)
        UnitTests::fail("Right Rotation failed");
    
    ba.shift(-2);
    ba = ba.subBytes(0, 4);
    
    ByteArray ba_s2((char[]){0x01, 0x01, 0x01, 0x01}, 4);
    if (ba != ba_s2)
        UnitTests::fail("Left Rotation failed");
    
    return true;
}

int main(int argc, const char * argv[]) {
    UnitTests tests;
    
    tests.addTest("testByteArray", testByteArray);
    tests.run();
    
    return 0;
}
