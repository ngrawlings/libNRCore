//
//  Log.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 15/12/2012.
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

#include "Log.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

namespace nrcore {

    #if LOG_THREAD_SAFE != 0 && THREADING_DISABLED == 0 && DEBUG_DISABLED == 0
    Mutex *Log::mutex = new Mutex();
    #endif

    Log::Log() {
    }

    Log::~Log() {
        LinkedListState<STREAM*> stms(&streams);
        STREAM* st;
        while (stms.length()) {
            st = stms.get();
            delete st;
            stms.remove();
        }
    }

    void Log::staticCleanUp() {
    #if LOG_THREAD_SAFE != 0 && THREADING_DISABLED == 0 && DEBUG_DISABLED == 0
        delete mutex;
    #endif
    }

    void Log::addStream(Stream* stream, const char *format, const char *time_format, int log_level) {
        STREAM *st = new STREAM;
        st->format = format;
        st->time_format = time_format;
        st->stream = stream;
        st->log_level = log_level;
        streams.add(st);
    }

    void Log::removeStream(Stream* stream) {
        streams.remove(stream);
    }

    void Log::log(int log_level, const char *format, ...) {
        va_list vars;
        va_start( vars, format );
        
        va_log(log_level, format, vars);
        
        va_end( vars );
    }

    void Log::va_log(int log_level, const char *format, va_list vars) {
    #if LOG_THREAD_SAFE != 0 && THREADING_DISABLED == 0 && DEBUG_DISABLED == 0
        mutex.lock();
    #endif
        
        time_t tm = time(0);
        struct tm _tm;
        
        gmtime_r(&tm, &_tm);
        
        LinkedListState<STREAM*> stms(&streams);
        int cnt = stms.length();
        STREAM* st;
        char timebuf[64];
        char logbuf[256];
        char final[256+64];
        
        vsnprintf(logbuf, 256, format, vars);
        
        while (cnt--) {
            st = stms.next();
            if (log_level <= st->log_level) {
                strftime(timebuf, 64, st->time_format, &_tm);
                snprintf(final, 256+64, st->format, timebuf, logbuf);
                st->stream->write(final, strlen(final));
            }
        }

    #if LOG_THREAD_SAFE != 0 && THREADING_DISABLED == 0 && DEBUG_DISABLED == 0
        mutex.release();
    #endif
    }
    
    void Log::log(const char *format, ...) {
        va_list vars;
        va_start( vars, format );
        
        log(Log::LOGLEVEL_ERROR, format, vars);
            
        va_end( vars );
    }
    
    void Log::va_log(const char *format, va_list vars) {
        va_log(Log::LOGLEVEL_ERROR, format, vars);
    }

    Log logger;
    
}
