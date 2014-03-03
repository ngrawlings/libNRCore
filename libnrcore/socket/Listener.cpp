//
//  Listener.cpp
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

#include "Listener.h"

#include <libnrcore/types.h>
#include <libnrcore/debug/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <err.h>

#include <libnrcore/threading/Thread.h>
#include "Socket.h"

#ifndef SERVER_LISTENER_BACKLOG
#define SERVER_LISTENER_BACKLOG 20
#endif

namespace nrcore {

    Listener::Listener() {
        
    }
    
    Listener::Listener(int listen_port, int opts) {
        listen(listen_port, opts, INADDR_ANY, in6addr_any);
    }
    
    Listener::Listener(unsigned int ipv4_interface, in6_addr ipv6_interface, int listen_port, int opts) : Task("Listener") {
        listen(listen_port, opts, ipv4_interface, ipv6_interface);
    }

    Listener::~Listener() {
        breakEventLoop();
        stop();
        
        if (ev_base)
            event_base_free(ev_base);
        
        LOG(Log::LOGLEVEL_NOTICE, "Listener Destroyed");
    }
    
    void Listener::listen(int listen_port, int opts, unsigned int ipv4_interface, in6_addr ipv6_interface) {
        ipv4_fd = 0;
        ipv6_fd = 0;
        ev_ipv4_accept = 0;
        ev_ipv6_accept = 0;
        
        ev_base = event_base_new();
        
        thread = 0;
        
        if (opts & LISTENER_OPTS_IPV4) {
            if (!ipv4listen(ipv4_interface, listen_port) && (opts & LISTENER_OPTS_IPV4_REQUIRED))
                throw "IPV4 Bind Failed";
        }
        if (opts & LISTENER_OPTS_IPV6) {
            if (!ipv6listen(ipv6_interface, listen_port) && (opts & LISTENER_OPTS_IPV6_REQUIRED))
                throw "IPV6 Bind Failed";
        }
        struct timeval tv;
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        ev_schedule = evtimer_new(ev_base, ev_schedule_tick, this);
        evtimer_add(ev_schedule, &tv);
    }
    
    void Listener::stop() {
        ::close(ipv4_fd);
        ::close(ipv6_fd);
        
        struct event *tmp = ev_schedule;
        ev_schedule = 0;
        
        event_del(tmp);
        event_free(tmp);
        
        if (ev_ipv4_accept) {
            event_del(ev_ipv4_accept);
            event_free(ev_ipv4_accept);
            ev_ipv4_accept = 0;
        }
        
        if (ev_ipv6_accept) {
            event_del(ev_ipv6_accept);
            event_free(ev_ipv6_accept);
            ev_ipv6_accept = 0;
        }
        
        if (thread) {
            thread->signal(SIGINT);
            thread->waitUntilFinished();
        }
    }

    void Listener::runEventLoop(bool create_task) {
        if (!create_task)
            event_base_dispatch(ev_base);
        else
            Thread::runTask(this);
    }

    void Listener::breakEventLoop() {
        event_base_loopbreak(ev_base);
    }

    int Listener::setNonBlocking(int fd) {
        int flags;
            
        flags = fcntl(fd, F_GETFL);
        if (flags < 0)
            return flags;
        flags |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) < 0)
            return -1;
            
        return 0;
    }

    void Listener::onAccept(int fd, short ev, int addr_len) {
        int client_fd;
        unsigned char *addr;
        
        if (addr_len == 4) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0) {
                
                LOG(Log::LOGLEVEL_ERROR, "accept failed %d\r\n", errno);
                return;
            }
            
            addr = (unsigned char*)&client_addr.sin_addr;
            
            LOG(Log::LOGLEVEL_NOTICE, "New Connection fd %d -> %d.%d.%d.%d", client_fd, ((char*)&client_addr.sin_addr)[0]&0xFF, ((char*)&client_addr.sin_addr)[1]&0xFF, ((char*)&client_addr.sin_addr)[2]&0xFF, ((char*)&client_addr.sin_addr)[3]&0xFF);
            
        } else {
            struct sockaddr_in6 client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0) {
                
                LOG(Log::LOGLEVEL_ERROR, "accept failed %d\r\n", errno);
                return;
            }
            
            addr = (unsigned char*)&client_addr.sin6_addr;
            
            LOG(Log::LOGLEVEL_NOTICE, "New Connection fd %d", client_fd);
            
        }
        
        /* Set the client socket to non-blocking mode. */
        if (setNonBlocking(client_fd) < 0)
            LOG(Log::LOGLEVEL_ERROR, "failed to set client socket non-blocking");
        
    #ifdef SO_NOSIGPIPE
        int n = 1;
        setsockopt(client_fd, SOL_SOCKET, SO_NOSIGPIPE, &n, sizeof(n));
    #endif
        
        
        
        onNewConnection(ev_base, client_fd, addr, addr_len);
    }

    void Listener::onAcceptIpV4(int fd, short ev, void *arg) {
        reinterpret_cast<Listener*>(arg)->onAccept(fd, ev, 4);
    }

    void Listener::onAcceptIpV6(int fd, short ev, void *arg) {
        reinterpret_cast<Listener*>(arg)->onAccept(fd, ev, 6);
    }

    bool Listener::ipv4listen(unsigned int interface, int port) {
        /* Create our listening socket. */
        sockaddr_in listen_addr;
        ipv4_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (ipv4_fd < 0)
            err(1, "listen failed");
        memset(&listen_addr, 0, sizeof(listen_addr));
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = interface;
        listen_addr.sin_port = htons(port);
        
        int on = 1;
        setsockopt(ipv4_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
        
        if (::bind(ipv4_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < (int)0) {
            LOG(Log::LOGLEVEL_ERROR, "Failed to Bind");
            return false;
        }
        if (::listen(ipv4_fd, SERVER_LISTENER_BACKLOG) < 0)
            err(1, "listen failed");
        
        int reuseaddr_on = 1;
        setsockopt(ipv4_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on));
        
        if (setNonBlocking(ipv4_fd) < 0)
            err(1, "failed to set server socket to non-blocking");
        
        ev_ipv4_accept = event_new(ev_base, ipv4_fd, EV_READ|EV_PERSIST, onAcceptIpV4, this);
        event_add(ev_ipv4_accept, NULL);
        
        return true;
    }

    bool Listener::ipv6listen(in6_addr interface, int port) {
        /* Create our listening socket. */
        sockaddr_in6 listen_addr;
        ipv6_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (ipv6_fd < 0)
            err(1, "listen failed");
        memset(&listen_addr, 0, sizeof(listen_addr));
        listen_addr.sin6_family = AF_INET6;
        listen_addr.sin6_addr   = interface;
        listen_addr.sin6_port = htons(port);
        
        int on = 1;
        setsockopt(ipv6_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
        
        if (::bind(ipv6_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
            LOG(Log::LOGLEVEL_ERROR, "Failed to Bind");
            return false;
        }
        if (::listen(ipv6_fd, 5) < 0)
            err(1, "listen failed");
        
        int reuseaddr_on = 1;
        setsockopt(ipv6_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on));
        
        if (setNonBlocking(ipv6_fd) < 0)
            err(1, "failed to set server socket to non-blocking");
        
        ev_ipv6_accept = event_new(ev_base, ipv6_fd, EV_READ|EV_PERSIST, onAcceptIpV6, this);
        event_add(ev_ipv6_accept, NULL);
        
        return true;
    }

    void Listener::run() {
        thread = Thread::getThreadInstance();
        
        event_base_dispatch(ev_base);
        
        thread = 0;
        LOG(Log::LOGLEVEL_WARNING, "Event Loop Exiting");
    }
    
    void Listener::ev_schedule_tick(int fd, short ev, void *arg) {
        if (reinterpret_cast<Listener*>(arg)->ev_schedule) {
            Socket::releaseEventQueue();
            
            struct timeval tv;
            
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            
            evtimer_add(reinterpret_cast<Listener*>(arg)->ev_schedule, &tv);
        }
    }
    
}
