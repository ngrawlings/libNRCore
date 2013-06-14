//
//  Log.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 07/11/2012.
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

#ifndef PeerConnector_Log_h
#define PeerConnector_Log_h

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include <memory/LinkedList.h>
#include <memory/Stream.h>

namespace nrcore {

    class Log {
    public:
        typedef struct {
            const char *format;
            const char *time_format;
            Stream* stream;
        } STREAM;
        
        Log();
        virtual ~Log();
        
        void addStream(Stream* stream, const char *format, const char *time_format);
        void removeStream(Stream* stream);
        void setTimeStrFormat(const char *format);
        void log(const char *format, ...);
        void va_log(const char *format, va_list vars);
        
        static void staticCleanUp();
        
    private:
        LinkedList<STREAM*> streams;
        
    #if LOG_THREAD_SAFE != 0
        static pthread_mutex_t mutex;
    #endif
    };

    extern Log logger;
    
}

#endif
