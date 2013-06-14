//
//  StreamReader.cpp
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

#include "StringStreamReader.h"

#include <string.h>

namespace nrcore {

    StringStreamReader::StringStreamReader(Stream *stream) : Task("StringStreamReader") {
        this->stream = stream;
        _run = true;
    }

    StringStreamReader::~StringStreamReader() {
        _run = false;
        if (thread)
            thread->waitUntilFinished();
    }

    void StringStreamReader::runBlockingMode() {
        run();
    }

    void StringStreamReader::close() {
        _run = false;
        stream->close();
    }

    void StringStreamReader::run() {
        thread = Thread::getThreadInstance();
        
        char buf[1024];
        ssize_t read = 0, r;
        buf[0] = 0;
        while (_run) {
            if (read == 1024)
                read = 0;
            
            r = stream->read(&buf[read], 1024-read);
            
            if (r>0) {
                read+=r;
                buf[read] = 0;
                if (buf[read-1] == '\n') {
                    buf[read-1] = 0;
                    if (strlen(buf) > 1)
                        onLineRead(buf);
                    
                    buf[0] = 0;
                    read = 0;
                }
            } else
                break;
            
        }
        
        thread = 0;
    }

};