//
//  ReliableUdp.cpp
//  libNRCore
//
//  Created by Nyhl on 10/02/15.
//  Copyright (c) 2015 N G Rawlings. All rights reserved.
//

#include "ReliableUdp.h"

namespace nrcore {

    ReliableUdp::ReliableUdp(EventBase *event_base, String interface, Address::ADDRESS_TYPE iptype, unsigned short port) :
        UdpSocket(event_base, interface, iptype, port), read_buffer(16383), write_buffer(8192)
    {
        
    }

    ReliableUdp::~ReliableUdp() {
        
    }
    
    void ReliableUdp::onReceive() {
        UdpPacket udp = recv();
        const char* bytes = udp.bytes();
        
        const char flags = bytes[0];
        
        bytes++;
        
        if (flags & ACK_BIT) {
            
        }
    }

};