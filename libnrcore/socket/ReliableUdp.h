//
//  ReliableUdp.h
//  libNRCore
//
//  Created by Nyhl on 10/02/15.
//  Copyright (c) 2015 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__ReliableUdp__
#define __libNRCore__ReliableUdp__

#include "UdpSocket.h"
#include <libnrcore/memory/RingBuffer.h>

#define ACK_BIT     0x01
#define DATA_BIT    0x02

namespace nrcore {

    class ReliableUdp : public UdpSocket {
    public:
        ReliableUdp(EventBase *event_base, String interface, Address::ADDRESS_TYPE iptype, unsigned short port);
        virtual ~ReliableUdp();
        
    protected:
        void onReceive();
        
        virtual int received(const char *bytes, const int len) = 0;
        virtual void disconnected() {};
        
    private:
        /*
         Packet header multiple of 2 bytes
         first byte is data offset, if bit 31 is true, a second short is present with an ACK and the third short is the data length
         if it is false the next short is the data length
         header minimum 4 bytes max 6 bytes
         */
        
        typedef struct {
            unsigned int expected;
            unsigned int actual;
        } ACT;
        
        ACT ack; // Outgoing ACT
        unsigned int incoming_count;
        
        RingBuffer read_buffer;
        RingBuffer write_buffer;
    };
    
};

#endif /* defined(__libNRCore__ReliableUdp__) */
