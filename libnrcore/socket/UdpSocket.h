//
//  UdpSocket.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 19/04/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__UdpSocket__
#define __libNRCore__UdpSocket__

#include <errno.h>

#include <libnrcore/event/EventBase.h>

#include <libnrcore/io/Stream.h>
#include <libnrcore/memory/RefArray.h>
#include <libnrcore/memory/Memory.h>
#include <libnrcore/memory/ByteArray.h>
#include <libnrcore/memory/StaticArray.h>
#include <libnrcore/memory/StringList.h>
#include <libnrcore/threading/Thread.h>
#include <libnrcore/debug/Log.h>

#include <libnrcore/threading/TaskMutex.h>

#include "Address.h"

#if defined(__ANDROID__) || defined(__LINUX__)
#include <platform/linux/socket/Interfaces.h>
#elif defined(__APPLE__)
#include <platform/osx/socket/Interfaces.h>
#endif

namespace nrcore {

    class UdpSocket : public Stream {
    public:
        class UdpPacket {
        public:
            UdpPacket(Address from, unsigned short port, const char* bytes, int len);
            UdpPacket(const UdpPacket &packet);
            ~UdpPacket();
            
            Address& address();
            unsigned short port();
            const char* bytes();
            int length();
            
        protected:
            Address _from;
            unsigned short _port;
            RefArray<char> _bytes;
            int _len;
        };
        
        UdpSocket(EventBase *event_base, String interface, Address::ADDRESS_TYPE iptype, unsigned short port);
        virtual ~UdpSocket();
        
        UdpPacket recv();
        int send(Address addr, unsigned short port, const char* bytes, int len);
        
        bool joinMulticastGroup(Address local, Address group);
        
    protected:
        virtual void onReceive() = 0;
        
        static int create(String interface, Address::ADDRESS_TYPE iptype, unsigned short port);
        static bool getIpAddr(String interface, Address::ADDRESS_TYPE iptype, char *addr);
 
        static void ev_read(int fd, short ev, void *arg);
        
    private:
        Interfaces interfaces;

        EventBase *event_base;
        struct event *event_read;
        
        String interface;
        Address::ADDRESS_TYPE addr_type;
        int mtu;
    };

};

#endif /* defined(__libNRCore__UdpSocket__) */
