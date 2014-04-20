//
//  Address.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 25/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "Address.h"

#include <string.h>
#include <sys/mman.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>

namespace nrcore {

    Address::Address(ADDRESS_TYPE type, const char* addr) {
        this->type = type;
        
        if (type != DOMAIN) {
            if (type == ADDR) {
                unsigned char addr_bytes[sizeof(struct in6_addr)];
                
                memset(addr_bytes, 0, sizeof(struct in6_addr));
                
                if (inet_pton(AF_INET, addr, (in_addr*)addr_bytes)==1) {
                    this->type = IPV4;
                } else if (inet_pton(AF_INET6, addr, (in6_addr*)addr_bytes)==1) {
                    this->type = IPV6;
                } else {
                    struct hostent *hostEntry;
                    hostEntry = gethostbyname(addr);
                    if (hostEntry) {
                        if (hostEntry->h_addrtype == AF_INET) {
                            this->type = IPV4;
                            memcpy((in_addr*)addr_bytes, hostEntry->h_addr_list[0], hostEntry->h_length);
                        } else if (hostEntry->h_addrtype == AF_INET6) {
                            this->type = IPV6;
                            memcpy((in6_addr*)addr_bytes, hostEntry->h_addr_list[0], hostEntry->h_length);
                        }
                    } else
                        throw -1;
                }
                
                addr = (const char*)addr_bytes;
            }
            
            len = getAddrSize();
            this->addr = new char[len];
            memcpy(this->addr, addr, len);
            
        } else {
            
            len = (int)strlen(addr)+1;
            this->addr = new char[len];
            memcpy(this->addr, addr, len);
            
        }
        
    }
    
    Address::Address(const Address &addr) {
        type = addr.type;
        len = addr.len;
        this->addr = new char[len];
        memcpy(this->addr, addr.addr, len);
    }

    Address::~Address() {
        delete [] addr;
    }

    Address::ADDRESS_TYPE Address::getType() {
        return type;
    }

    int Address::getAddrSize() {
        switch (type) {
            case IPV4:
                return 4;
                
            case IPV6:
                return 16;
                
            case MAC:
                return 6;
                
            default:
                return 0;
        }
    }

    const char* Address::getAddr() {
        return addr;
    }
    
    Address::ADDRESS_TYPE Address::getType(const char* addr) {
        ADDRESS_TYPE ret = DOMAIN;
        
        char *addr_bytes = new char[sizeof(struct in6_addr)];
        
        memset(addr_bytes, 0, sizeof(struct in6_addr));
        
        if (inet_pton(AF_INET, addr, addr_bytes)==1)
            return IPV4;
        else if (inet_pton(AF_INET6, addr, addr_bytes)==1)
            return IPV6;
        
        delete [] addr_bytes;
        
        return ret;
    }
    
};
