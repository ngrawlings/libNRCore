//
//  Socks5Auth.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 17/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Socks5Auth__
#define __libNRCore__Socks5Auth__

namespace nrcore {
    
    class Socks5;
  
    class Socks5Auth {
    public:
        Socks5Auth(Socks5 *socks);
        virtual ~Socks5Auth();
        
    protected:
        Socks5 *socks;
        
        inline int send(const char *bytes, const int len);
    };
    
};

#endif /* defined(__libNRCore__Socks5Auth__) */
