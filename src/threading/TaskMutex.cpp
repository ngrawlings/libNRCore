//
//  TaskMutex.cpp
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

#include "TaskMutex.h"

namespace nrcore {

    TaskMutex::~TaskMutex() {
        if (wait_lock)
            delete wait_lock;
        
        if (wait_lock_trigger)
            delete wait_lock_trigger;
        
    }

    void TaskMutex::setQueued() {
        state = QUEUED;
    }

    void TaskMutex::setWaiting() {
        if (wait_lock && wait_lock_trigger) {
            wait_lock_trigger->broadcast();
        }
        state = WAITING;
    }

    void TaskMutex::setBusy() {
        state = BUSY;
    }

    TaskMutex::STATE TaskMutex::getState() {
        return state;
    }

    void TaskMutex::waitUntilFinished() {
        wait_lock = new Mutex();
        wait_lock_trigger = new ThreadWaitCondition();
        
        wait_lock->wait(wait_lock_trigger);
    }

}