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
#include <libnrcore/memory/Memory.h>
#include <libnrcore/memory/StaticArray.h>
#include <libnrcore/memory/StringList.h>
#include <libnrcore/threading/Thread.h>
#include <libnrcore/debug/Log.h>

#include <libnrcore/threading/TaskMutex.h>

#include "Address.h"

namespace nrcore {

    class UdpSocket : public Stream {
    public:
        UdpSocket(EventBase *event_base, String interface, Address::ADDRESS_TYPE iptype, unsigned short port);
        virtual ~UdpSocket();
        
        static StringList getInterfaces();
        
        int send(Address addr, int port, const char* bytes, int len);
        
    protected:
        static int create(String interface, Address::ADDRESS_TYPE iptype, unsigned short port);
        static bool getIpAddr(String interface, Address::ADDRESS_TYPE iptype, char *addr);
 
        static void ev_read(int fd, short ev, void *arg);
        
    private:
        EventBase *event_base;
        struct event *event_read;
        
        Address::ADDRESS_TYPE addr_type;
    };

};

#endif /* defined(__libNRCore__UdpSocket__) */
