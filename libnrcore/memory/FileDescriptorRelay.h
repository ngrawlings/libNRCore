//
//  FileDescriptorRelay.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 21/05/2013.
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

#ifndef __PeerConnectorCore__FileDescriptorRelay__
#define __PeerConnectorCore__FileDescriptorRelay__

extern "C" {
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/event_compat.h>
}

#include <libnrcore/base/Object.h>

namespace nrcore {

    class FileDescriptorRelay : public Object {
    public:
        FileDescriptorRelay(event_base *ev_base, int fd1, int fd2);
        virtual ~FileDescriptorRelay();
        
        bool isOpen();
        
    private:
        struct event_base *ev_base;
        int fd1, fd2;
        struct event *fd1_event, *fd2_event;
        
        int setNonBlocking(int fd);
        
        void recv(int fd);
        static void static_recv(int fd, short ev, void *arg);
    };
    
};

#endif /* defined(__PeerConnectorCore__FileDescriptorRelay__) */
