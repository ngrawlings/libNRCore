//
//  TaskMutex.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 26/03/2013.
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

#ifndef __PeerConnectorCore__TaskMutex__
#define __PeerConnectorCore__TaskMutex__

#include "Mutex.h"
#include "ThreadWaitCondition.h"

namespace nrcore {

    class TaskMutex : public Mutex {
    public:
        enum STATE {
            WAITING,
            BUSY,
            QUEUED
        };
        
    public:
        TaskMutex(const char *tag=0) : Mutex(tag), state(WAITING), wait_lock(0), wait_lock_trigger(0) {}
        virtual ~TaskMutex();
        
        void setQueued();
        void setWaiting();
        void setBusy();
        STATE getState();
        void reset();
        
        void waitUntilFinished();
        
    private:
        STATE state;
        Mutex *wait_lock;
        ThreadWaitCondition *wait_lock_trigger;
    };
    
}

#endif /* defined(__PeerConnectorCore__TaskMutex__) */
