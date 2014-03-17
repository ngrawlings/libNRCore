//
//  Socks5AuthPlain.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 17/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Socks5AuthPlain__
#define __libNRCore__Socks5AuthPlain__

#include "Socks5Auth.h"

namespace nrcore {
  
    class Socks5AuthBasic : public Socks5Auth {
    public:
        Socks5AuthBasic(Socks5 *socks);
        virtual ~Socks5AuthBasic();
        
    };
    
};

#endif /* defined(__libNRCore__Socks5AuthPlain__) */
