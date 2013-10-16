//
//  FileDescriptorRelay.cpp
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

#include "FileDescriptorRelay.h"

#include <libnrcore/types.h>

#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

namespace nrcore {

    FileDescriptorRelay::FileDescriptorRelay(event_base *ev_base, int fd1, int fd2) {
        fd1_event = event_new(ev_base, fd1, EV_READ|EV_PERSIST, static_recv, this);
        fd2_event = event_new(ev_base, fd2, EV_READ|EV_PERSIST, static_recv, this);
        event_add(fd1_event, NULL);
        event_add(fd2_event, NULL);
    }

    FileDescriptorRelay::~FileDescriptorRelay() {
        event_del(fd1_event);
        event_del(fd2_event);
        event_free(fd1_event);
        event_free(fd2_event);
    }

    bool FileDescriptorRelay::isOpen()
    {
        return (fcntl(fd1, F_GETFD) != -1 || errno != EBADF) && (fcntl(fd2, F_GETFD) != -1 || errno != EBADF);
    }

    int FileDescriptorRelay::setNonBlocking(int fd) {
        int flags;
        
        flags = fcntl(fd, F_GETFL);
        if (flags < 0)
            return flags;
        flags |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) < 0)
            return -1;
        
        return 0;
    }

    void FileDescriptorRelay::recv(int fd) {
        char buf[32];
        size_t r = read(fd, buf, 32);
        
        if (r > 0) {
            if (fd == fd1) {
                write(fd2, buf, r);
            } else if (fd == fd2) {
                write(fd1, buf, r);
            }
        }
    }

    void FileDescriptorRelay::static_recv(int fd, short ev, void *arg) {
        reinterpret_cast<FileDescriptorRelay*>(arg)->recv(fd);
    }

};