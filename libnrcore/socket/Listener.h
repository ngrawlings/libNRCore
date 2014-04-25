//
//  Listener.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 01/03/2013.
//  Copyright (c) 2013. All rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// For affordable commercial licensing please contact nyhl@ngrawlings.com
//

#ifndef __PeerConnector__Listener__
#define __PeerConnector__Listener__

#include <libnrcore/threading/Thread.h>
#include <libnrcore/threading/Task.h>
#include <libnrcore/event/EventBase.h>

#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <netinet/in.h>
#include <netinet/in6.h>

#define LISTENER_OPTS_IPV4              1
#define LISTENER_OPTS_IPV4_REQUIRED     2
#define LISTENER_OPTS_IPV6              4
#define LISTENER_OPTS_IPV6_REQUIRED     8

namespace nrcore {

    class Listener : public Task {
    public:
        Listener();
        Listener(int listen_port, int opts, EventBase *event_base=0);
        Listener(unsigned int ipv4_interface, const in6_addr *ipv6_interface, int listen_port, int opts, EventBase *event_base=0);
        virtual ~Listener();
        
        void listen(int listen_port, int opts, unsigned int interface=INADDR_ANY, const in6_addr *ipv6_interface=&in6addr_any, EventBase *event_base=0);
        void stop();
        
        void runEventLoop(bool create_task=false);
        void breakEventLoop();
        
    protected:
        
        virtual void onNewConnection(EventBase *event_base, int fd, unsigned char *addr, int addr_sz) = 0;
        virtual void run();
        
    private:
        int ipv4_fd;
        int ipv6_fd;
        
        Thread *thread;
        
        EventBase *event_base;
        bool evbase_allocated;
        
        struct event *ev_ipv4_accept;
        struct event *ev_ipv6_accept;
        
        int setNonBlocking(int fd);
        
        void onAccept(int fd, short ev, int addr_len);
        
        static void onAcceptIpV4(int fd, short ev, void *arg);
        static void onAcceptIpV6(int fd, short ev, void *arg);
        
        bool ipv4listen(unsigned int interface, int port);
        bool ipv6listen(in6_addr interface, int port);
        
    };
    
}

#endif /* defined(__PeerConnector__Listener__) */
