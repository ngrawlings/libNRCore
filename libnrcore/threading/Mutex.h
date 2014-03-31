//
//  Mutex.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 12/02/2013.
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

#ifndef __PeerConnector__Mutex__
#define __PeerConnector__Mutex__

#include <libnrcore/types.h>
#include <pthread.h>
#include "ThreadWaitCondition.h"

namespace nrcore {

    class Mutex {
    public:
        Mutex(const char *tag=0, bool manage=true);
        virtual ~Mutex();
        
        bool lock(long timeout=0, const char* lock_tag=0);
        void wait(ThreadWaitCondition *cond, int usecs=0);
        bool tryLock(const char* lock_tag=0);
        void release();
        bool isLocked();
        bool isLockedByMe();
        thread_t getOwner();
        
        const char* tag();
        const char* lockTag();
        
        bool isManaged();

    private:
        thread_t owner;
        thread_mutex_t mutex;
        const char* _tag;
        const char* lock_tag;
        bool manage;
    };
    
}

#endif /* defined(__PeerConnector__Mutex__) */
