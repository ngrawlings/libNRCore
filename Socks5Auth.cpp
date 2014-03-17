//
//  Socks5Auth.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 17/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "Socks5Auth.h"

#include <libnrcore/socket/Socks5.h>

namespace nrcore {
  
    Socks5Auth::Socks5Auth(Socks5 *socks) {
        this->socks = socks;
    }
    
    Socks5Auth::~Socks5Auth() {
        
    }
    
    int Socks5Auth::send(const char *bytes, const int len) {
        return socks->send(bytes, len);
    }
    
};