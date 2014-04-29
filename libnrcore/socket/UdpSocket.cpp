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
    
    String UdpSocket::getInterfaceAddr(String interface, Address::ADDRESS_TYPE iptype) {
        return "";
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
            ((sockaddr_in*)addr_buf)->sin_len = sizeof(sockaddr_in);
            ((sockaddr_in*)addr_buf)->sin_family = AF_INET;
            memcpy(&((sockaddr_in*)addr_buf)->sin_addr, addr.getAddr(), sizeof(in_addr));
            ((sockaddr_in*)addr_buf)->sin_port = htons(port);
        } else {
            ((sockaddr_in6*)addr_buf)->sin6_len = sizeof(sockaddr_in6);
            ((sockaddr_in6*)addr_buf)->sin6_family = AF_INET6;
            memcpy(&((sockaddr_in6*)addr_buf)->sin6_addr, addr.getAddr(), sizeof(in6_addr));
            ((sockaddr_in*)addr_buf)->sin_port = htons(port);
        }
        
        return (int)sendto(fd, bytes, len, 0, (sockaddr*)addr_buf, addr_type == Address::IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));
    }
    
    bool UdpSocket::joinMulticastGroup(Address local, Address group) {
        if (addr_type == Address::IPV4) {
                
            struct ip_mreq g;
            memcpy(&g.imr_multiaddr.s_addr, group.getAddr(), sizeof(in_addr));
            memcpy(&g.imr_interface.s_addr, local.getAddr(), sizeof(in_addr));
            if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&g, sizeof(ip_mreq)) == 0)
                return true;
                
        } else {
            
            // TODO: Probably will not work in all cases
            struct ifreq ifr;
            if (ioctl(fd, 0x8933 /* SIOCGIFINDEX */, &ifr) == -1) {
                return false;
            }
                
            struct ipv6_mreq g;
            memcpy(&g.ipv6mr_multiaddr, group.getAddr(), sizeof(in6_addr));
            g.ipv6mr_interface = ifr.ifr_ifru.ifru_intval;
            if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&g, sizeof(g)) == 0)
                return true;
                
        }
        
        return false;
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
        reinterpret_cast<UdpSocket*>(arg)->onReceive();
        event_add(reinterpret_cast<UdpSocket*>(arg)->event_read, NULL);
    }
    
};
