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

#if THREADING_DISABLED == 0
#include <libnrcore/threading/Mutex.h>
#endif

#include <libnrcore/memory/LinkedList.h>
#include <libnrcore/io/Stream.h>

#if DEBUG_DISABLED == 0

#define LOG(X, Y, ...) logger.log(X, Y, ##__VA_ARGS__)
#define C(x) x

#else

#define LOG(X, Y, ...) ;
#define C(x) ""

#endif

namespace nrcore {

    class Log {
    public:
        typedef struct {
            const char *format;
            const char *time_format;
            int log_level;
            Stream* stream;
        } STREAM;
        
        typedef enum {
            LOGLEVEL_ERROR      = 0,
            LOGLEVEL_WARNING    = 1,
            LOGLEVEL_NOTICE     = 2,
            LOGLEVEL_VERBOSE    = 3
        } LOGLEVEL;
        
        Log();
        virtual ~Log();
        
        void addStream(Stream* stream, const char *format, const char *time_format, int log_level);
        void removeStream(Stream* stream);
        void setTimeStrFormat(const char *format);
        void log(int log_level, const char *format, ...);
        void va_log(int log_level, const char *format, va_list vars);
        
        void log(const char *format, ...);
        void va_log(const char *format, va_list vars);
        
        static void staticCleanUp();
        
    private:
        LinkedList<STREAM*> streams;
        
    #if LOG_THREAD_SAFE != 0 && !defined(THREADING_DISABLED) && !defined(DEBUG_DISABLED)
        static Mutex *mutex;
    #endif
        
        
    };

    extern Log logger;
    
}

#endif
