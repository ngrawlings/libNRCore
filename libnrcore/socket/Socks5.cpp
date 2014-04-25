//
//  Socks5.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 16/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "Socks5.h"

#include <libnrcore/memory/String.h>
#include <libnrcore/memory/StringList.h>

#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <netinet/in.h>
#include <netinet/in6.h>

namespace nrcore {

    Socks5::Socks5(EventBase *event_base, const char* addr) :
                Socket(event_base, addr),
                state(INITIATED),
                selected_auth_method(NONE)
    {
        
    }
    
    Socks5::Socks5(EventBase *event_base, const char* addr, unsigned short port) :
                Socket(event_base, addr, port),
                state(INITIATED),
                selected_auth_method(NONE)
    {
        
    }
    
    Socks5::Socks5(EventBase *event_base, int fd) :
                Socket(event_base, fd),
                state(INITIATED),
                selected_auth_method(NONE)
    {
        
    }

    Socks5::~Socks5() {
        
    }
    
    void Socks5::addAuthenticationMethod(const char method) {
        supported_auth_methods.append((void*)&method, 1);
    }
    
    void Socks5::init() {
        unsigned char cnt = (unsigned char)supported_auth_methods.length();
        unsigned char _req[] = {0x05, cnt};
        ByteArray req(_req, 2);
        
        req.append(supported_auth_methods);
        
        Socket::send((char*)req.operator const char *(), 2+cnt);
    }
    
    bool Socks5::beforeReceived(const char *bytes, const int len) {
        switch (state) {
            case INITIATED:
                
                if (len == 2) {
                    if (bytes[0] == 0x05 && ((unsigned char)bytes[1]) != 0xFF) {
                        selected_auth_method = bytes[1];
                        if (selected_auth_method != 0x00) {
                            state = AUTHENTICATING;
                            onAuthentication(selected_auth_method);
                        } else {
                            state = AUTHENTICATED;
                            onAuthenticated();
                        }
                    } else
                        close();
                } else
                    close();
                
                break;
                
            case AUTHENTICATING:
                
                if (len == 2) {
                    if (bytes[0] == 0x05) {
                        if (!bytes[1]) {
                            state = AUTHENTICATED;
                            onAuthenticated();
                        } else
                            close();
                    } else
                        close();
                } else
                    close();
                
                break;
                
            case AUTHENTICATED:
                
                if (bytes[0] == 0x05) {
                    if (bytes[1] == 0x00) {
                        state = CONNECTED;
                        onConnected();
                    } else
                        close();
                } else
                    close();
                
                break;
                
            case CONNECTED:
                return true;
        }
        return false;
    }
    
    int Socks5::send(const char *bytes, const int len) {
        if (state == CONNECTED)
            return Socket::send(bytes, len);
        LOG(Log::LOGLEVEL_WARNING, "Socks5 sending before connected");
        return 0;
    }
    
    Socks5::STATE Socks5::getSocksState() {
        return state;
    }
    
    void Socks5::authenticate(String username, String password) {
        
    }
    
    void Socks5::connect(String addr) {
        String address(addr);
        unsigned short port;
        String ip = address.extract("[", "]");
        if (ip.length() == 0) {
            StringList parts(address, ":");
            port = atoi(parts[1]);
            address = parts[0];
        } else {
            port = atoi(address.substr((int)ip.length()+3));
            address = ip;
        }
        
        connect(address, port);
    }
    
    void Socks5::connect(String addr, unsigned short port) {
        char addr_bytes[sizeof(struct in6_addr)];
        Address::ADDRESS_TYPE addr_type = Address::getType(addr);
        const char _req[] = {0x05, 0x01, 0x00, (char)addr_type};
        ByteArray req(_req, 4);
        if (addr_type == Address::DOMAIN) {
            unsigned char l = (unsigned char)addr.length();
            req.append(&l, 1);
            req.append(addr.operator char *(), (int)addr.length());
        } else {
            req.append(addr_bytes, (addr_type == Address::IPV4 ? sizeof(struct in_addr) : sizeof(struct in6_addr)));
        }
        
        port = htons(port);
        req.append(&port, 2);
        Socket::send(req.operator const char *(), (int)req.length());
    }

};
