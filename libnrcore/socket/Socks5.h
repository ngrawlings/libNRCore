//
//  Socks5.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 16/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Socks5__
#define __libNRCore__Socks5__

#include "Socket.h"

#include <libnrcore/memory/ByteArray.h>

namespace nrcore {

    class Socks5 : public Socket {
    public:
        enum STATE {
            INITIATED,
            AUTHENTICATING,
            AUTHENTICATED,
            CONNECTED
        };
        
        enum AUTH_METHOD {
            NONE                = 0x00,
            PLAIN               = 0x02,
        };
        
        Socks5(EventBase *event_base, const char* addr);
        Socks5(EventBase *event_base, const char* addr, unsigned short port);
        Socks5(EventBase *event_base, int fd);
        virtual ~Socks5();
        
        void addAuthenticationMethod(const char method);
        
        void init();
        void authenticate(String username, String password);
        void connect(String addr);
        void connect(String addr, unsigned short port);
        
        virtual int send(const char *bytes, const int len);
        
        STATE getSocksState();
        
    protected:
        bool beforeReceived(const char *bytes, const int len);
   
        virtual void onAuthentication(const unsigned char method) = 0;
        virtual void onAuthenticated() = 0;
        virtual void onConnected() = 0;
    
    private:
        STATE state;
        unsigned char selected_auth_method;
        ByteArray supported_auth_methods;
    };
    
};

#endif /* defined(__libNRCore__Socks5__) */
