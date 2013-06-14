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

#include <Threading/Thread.h>
#include <Threading/Task.h>

extern "C" {
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/event_compat.h>
}


#define LISTENER_OPTS_IPV4              1
#define LISTENER_OPTS_IPV4_REQUIRED     2
#define LISTENER_OPTS_IPV6              4
#define LISTENER_OPTS_IPV6_REQUIRED     8

namespace nrcore {

    class Listener : public Task {
    public:
        Listener(int listen_port, int opts);
        virtual ~Listener();
        
        void stop();
        
        void runEventLoop(bool create_task=false);
        void breakEventLoop();
        
    protected:
        
        virtual void onNewConnection(event_base *ev_base, int fd, unsigned char *addr, int addr_sz) = 0;
        virtual void run();
        
    private:
        int ipv4_fd;
        int ipv6_fd;
        
        Thread *thread;
        
        struct event_base *ev_base;
        struct event *ev_ipv4_accept;
        struct event *ev_ipv6_accept;
        
        int setNonBlocking(int fd);
        
        void onAccept(int fd, short ev, int addr_len);
        
        static void onAcceptIpV4(int fd, short ev, void *arg);
        static void onAcceptIpV6(int fd, short ev, void *arg);
        
        bool ipv4listen(int port);
        bool ipv6listen(int port);

    };
    
}

#endif /* defined(__PeerConnector__Listener__) */
