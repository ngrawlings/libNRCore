//
//  Stream.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 19/05/2013.
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

#include "Stream.h"
#include <libnrcore/Debug/Log.h>
#include <fcntl.h>
#include <errno.h>

namespace nrcore {

    Stream::Stream(int fd) {
        this->fd = fd;
    }

    Stream::~Stream() {
        close();
    }

    void Stream::close() {
        ::close(fd);
    }

    void Stream::setFd(int fd) {
        this->fd = fd;
    }

    int Stream::getFd() {
        return fd;
    }

    bool Stream::isValid() {
        return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
    }

    ssize_t Stream::write(const char* buf, size_t sz) {
        if (fd<0)
            return 0;
        
        ssize_t ret = ::write(fd, buf, sz);
        fsync(fd);
        return ret;
    }

    ssize_t Stream::read(char* buf, size_t sz) {
        if (fd < 0)
            return 0;
        
        return ::read(fd, buf, sz);
    }

};