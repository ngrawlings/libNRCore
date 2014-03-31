//
//  Buffer.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 14/05/2013.
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

#ifndef __PeerConnectorCore__Buffer__
#define __PeerConnectorCore__Buffer__

#include <libnrcore/threading/Mutex.h>
#include <libnrcore/memory/ByteArray.h>
#include <libnrcore/memory/LinkedList.h>

namespace nrcore {
    
    class Socket;

    class Buffer {
    public:
        Buffer(Socket* sock);
        virtual ~Buffer();
        
        void append(const char* bytes, int len);
        int send();
        int length();
        
    private:
        Socket *sock;
        LinkedList<ByteArray*> buffer;
        Mutex mutex;
        int len;
    };
    
};

#endif /* defined(__PeerConnectorCore__Buffer__) */
