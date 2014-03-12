//
//  FifoStream.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 22/05/2013.
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

#include "FifoStream.h"

#include <libnrcore/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

namespace nrcore {

    FifoStream::FifoStream(const char *path, int mode) : Stream(-1), state(WAITING) {
        this->path = path;
        this->mode = mode;
        
        if (mkfifo(path, 0666)) {
            if (errno != EEXIST)
                throw "Faield to create fifo";
        }
        Thread::runTask(this);
    }

    FifoStream::~FifoStream() {
        if (thread)
            thread->waitUntilFinished();
    }

    ssize_t FifoStream::write(const char* buf, size_t sz) {
        if (state == OPEN)
            return Stream::write(buf, sz);
        return 0;
    }

    ssize_t FifoStream::read(char* buf, size_t sz) {
        if (state == OPEN)
            return Stream::read(buf, sz);
        return 0;
    }

    FifoStream::STATE FifoStream::getState() {
        return state;
    }

    void FifoStream::run() {
        thread = Thread::getThreadInstance();
        
        fd = open(path, mode);
        
        if (getFd())
            state = OPEN;
        else
            state = CLOSED;
        
        onStateChanged(state);
        
        thread = 0;
    }

}
