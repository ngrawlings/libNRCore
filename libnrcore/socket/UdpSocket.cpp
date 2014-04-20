//
//  UdpSocket.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 19/04/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "UdpSocket.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

namespace nrcore {
    
    UdpSocket::UdpSocket(EventBase *event_base, String interface, Address::ADDRESS_TYPE iptype, unsigned short port) : Stream(0) {
        this->event_base = event_base;
        this->addr_type = iptype;
        
        fd = create(interface, iptype, port);
        if (!fd)
            throw -1;
        
        event_read = event_new(event_base->getEventBase(), fd, EV_READ|EV_PERSIST, ev_read, (void*)this);
        event_add(event_read, NULL);
    }
    
    UdpSocket::~UdpSocket() {
        if (fd)
            close();
        
        event_del(event_read);
        event_free(event_read);
    }

    StringList UdpSocket::getInterfaces() {
        StringList interface_list;
        struct ifaddrs *ifap, *ifa;
        
        getifaddrs (&ifap);
        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr->sa_family==AF_INET)
                interface_list.append(ifa->ifa_name);
        }
        freeifaddrs(ifap);
        
        return interface_list;
    }
    
    int UdpSocket::send(Address addr, int port, const char* bytes, int len) {
        
    }
    
    int UdpSocket::create(String interface, Address::ADDRESS_TYPE iptype, unsigned short port) {
        char buffer[sizeof(sockaddr_in6)];
        if (getIpAddr(interface, iptype, buffer)) {
            int fd = socket((iptype==Address::IPV4 ? AF_INET : AF_INET6), SOCK_DGRAM, 0);
            if (iptype==Address::IPV4)
                ((sockaddr_in*)buffer)->sin_port = htons(port);
            else
                ((sockaddr_in6*)buffer)->sin6_port = htons(port);
            
            if (bind(fd, (struct sockaddr *)buffer, iptype==Address::IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)) == 0) {
                return fd;
            }
        }
        return 0;
    }
    
    bool UdpSocket::getIpAddr(String interface, Address::ADDRESS_TYPE iptype, char *addr) {
        bool ret = false;
        struct ifaddrs *ifap, *ifa;
        
        getifaddrs (&ifap);
        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
            if (interface.equals(ifa->ifa_name, false) && ((iptype == Address::IPV4 && ifa->ifa_addr->sa_family==AF_INET) || (iptype == Address::IPV6 && ifa->ifa_addr->sa_family==AF_INET6))) {
                memcpy(addr, ifa->ifa_addr, sizeof(sockaddr_in6));
                ret = true;
                break;
            }
        }
        freeifaddrs(ifap);
        
        return ret;
    }
    
    void UdpSocket::ev_read(int fd, short ev, void *arg) {
        printf("udp\r\n");
    }
    
};