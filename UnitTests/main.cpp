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
#include <libnrcore/memory/HashMap.h>
#include <libnrcore/memory/String.h>
#include <libnrcore/memory/RingBuffer.h>

#include <libnrcore/encoding/Base58.h>

using namespace nrcore;

bool testByteArrayShift() {
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

bool testByteArrayAllocation() {
    ByteArray ba;
    
    for (int i=0; i<1024; i++) {
        ba.append(Memory::getRandomBytes(1024));
        ba = ba.subBytes(512);
    }

    return true;
}

bool testHashMap() {
    HashMap<String> *map = new HashMap<String>();
    
    for (int i=100; i<65536; i++) {
        map->set(String("%").arg(i), String("%").arg(i));
    }
    
    delete map;
    
    return true;
}

bool testRingBuffer() {
    RingBuffer buf(128);
    
    char data[256];
    for (int i=0; i<256; i++)
        data[i] = i%64;
    
    for (int i=0; i<256; i++) {
        int appended = (int)buf.append(data, 256);
        Memory mem = buf.fetch(64);
        printf("%d -> ", appended);
        for (int x=0; x<mem.length(); x++) {
            printf("%02X ", mem[x]);
        }
        printf("\n");
    }
    return true;
}

bool testBase58() {
    Memory enc = Base58::encode(Memory("12345678901234567890123456789012", 32));
    Memory dec = Base58::decode(enc);
    return true;
}

int main(int argc, const char * argv[]) {
    UnitTests tests;
    
    tests.addTest("testByteArrayShift", testByteArrayShift);
    tests.addTest("testByteArrayAllocation", testByteArrayAllocation);
    tests.addTest("testHashMap", testHashMap);
    tests.addTest("testRingBuffer", testRingBuffer);
    tests.addTest("testBase58", testBase58);
    tests.run();
    
    return 0;
}
