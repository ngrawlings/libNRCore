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
#include <net/if.h>
#include <sys/ioctl.h>

namespace nrcore {
    
    UdpSocket::UdpPacket::UdpPacket(Address from, unsigned short port, const char* bytes, int len) : _from(from), _bytes(0) {
        _port = port;
        _len = len;
        char *b = new char[len];
        
        memcpy(b, bytes, len);
        
        _bytes = RefArray<char>(b);
    }
    
    UdpSocket::UdpPacket::UdpPacket(const UdpPacket &packet) : _from(packet._from), _bytes(packet._bytes) {
        _port = packet._port;
        _len = packet._len;
    }
    
    UdpSocket::UdpPacket::~UdpPacket() {

    }
    
    Address& UdpSocket::UdpPacket::address() {
        return _from;
    }
    
    unsigned short UdpSocket::UdpPacket::port() {
        return _port;
    }
    
    const char* UdpSocket::UdpPacket::bytes() {
        return _bytes.getPtr();
    }
    
    int UdpSocket::UdpPacket::length() {
        return _len;
    }
    
    UdpSocket::UdpSocket(EventBase *event_base, String interface, Address::ADDRESS_TYPE iptype, unsigned short port) : Stream(0) {
        this->event_base = event_base;
        this->addr_type = iptype;
        this->interface = interface;
        
        fd = create(interface, iptype, port);
        if (!fd)
            throw -1;
        
        event_read = event_new(event_base->getEventBase(), fd, EV_READ, ev_read, (void*)this);
        event_add(event_read, NULL);
        
        mtu = 4096;
        
        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifreq));
        ifr.ifr_addr.sa_family = iptype == Address::IPV4 ? AF_INET : AF_INET6;
        strcpy(ifr.ifr_name, interface);
        
        if (ioctl(fd, SIOCGIFMTU, (caddr_t)&ifr) < 0)
            mtu = ifr.ifr_ifru.ifru_mtu;
    }
    
    UdpSocket::~UdpSocket() {
        if (fd)
            close();
        
        event_del(event_read);
        event_free(event_read);
    }
    
    UdpSocket::UdpPacket UdpSocket::recv() {
        socklen_t slen = sizeof(sockaddr_in6);
        char addr_buf[slen];
        char buf[mtu];
        int len;
        
        memset(addr_buf, 0, sizeof(sockaddr_in6));
        
        len = (int)recvfrom(fd, buf, mtu, 0, (sockaddr*)addr_buf, &slen);
        
        char *addr = ((char*)(addr_type == Address::IPV4 ? (char*)&((sockaddr_in*)addr_buf)->sin_addr : (char*)&((sockaddr_in6*)addr_buf)->sin6_addr));
        unsigned short port = htons((addr_type == Address::IPV4 ? ((sockaddr_in*)addr_buf)->sin_port : ((sockaddr_in6*)addr_buf)->sin6_port));
        
        return UdpPacket(Address(addr_type, addr), port, buf, len);
    }
    
    int UdpSocket::send(Address addr, unsigned short port, const char* bytes, int len) {
        if (addr.getType() != addr_type)
            return 0;
        
        char addr_buf[sizeof(sockaddr_in6)];
        memset(addr_buf, 0, sizeof(sockaddr_in6));
        
        if (addr_type == Address::IPV4) {
#if !defined(__ANDROID__)
            ((sockaddr_in*)addr_buf)->sin_len = sizeof(sockaddr_in);
#endif
            ((sockaddr_in*)addr_buf)->sin_family = AF_INET;
            memcpy(&((sockaddr_in*)addr_buf)->sin_addr, addr.getAddr(), sizeof(in_addr));
            ((sockaddr_in*)addr_buf)->sin_port = htons(port);
        } else {
#if !defined(__ANDROID__)
            ((sockaddr_in6*)addr_buf)->sin6_len = sizeof(sockaddr_in6);
#endif
            ((sockaddr_in6*)addr_buf)->sin6_family = AF_INET6;
            memcpy(&((sockaddr_in6*)addr_buf)->sin6_addr, addr.getAddr(), sizeof(in6_addr));
            ((sockaddr_in*)addr_buf)->sin_port = htons(port);
        }
        
        return (int)sendto(fd, bytes, len, 0, (sockaddr*)addr_buf, addr_type == Address::IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));
    }
    
    int UdpSocket::create(String interface, Address::ADDRESS_TYPE iptype, unsigned short port) {
        char buffer[sizeof(sockaddr_in6)];
        
        memset(buffer, 0, sizeof(sizeof(sockaddr_in6)));
        
        if (interface.equals("ANY", true) || getIpAddr(interface, iptype, buffer)) {
            int fd = socket((iptype==Address::IPV4 ? AF_INET : AF_INET6), SOCK_DGRAM, 0);
            
            if (iptype==Address::IPV4) {
                
                ((sockaddr_in*)buffer)->sin_family = AF_INET;
                ((sockaddr_in*)buffer)->sin_port = htons(port);
                
                if (interface.equals("ANY", true))
                    ((sockaddr_in*)buffer)->sin_addr.s_addr = INADDR_ANY;
                
            } else {
                ((sockaddr_in6*)buffer)->sin6_family = AF_INET6;
                ((sockaddr_in6*)buffer)->sin6_port = htons(port);
                
                if (interface.equals("ANY", true))
                    ((sockaddr_in6*)buffer)->sin6_addr = in6addr_any;
            }
            
            if (bind(fd, (struct sockaddr *)buffer, iptype==Address::IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)) == 0) {
                return fd;
            }
        }
        return 0;
    }
    
    void UdpSocket::ev_read(int fd, short ev, void *arg) {
        reinterpret_cast<UdpSocket*>(arg)->onReceive();
        event_add(reinterpret_cast<UdpSocket*>(arg)->event_read, NULL);
    }
    
};
