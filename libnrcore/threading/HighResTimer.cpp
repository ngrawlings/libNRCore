//
//  HighResTimer.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 16/06/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#include "HighResTimer.h"

namespace nrcore {

    HighResTimer::HighResTimer(useconds_t interval) {
        this->interval = interval;
    }

    HighResTimer::~HighResTimer() {
        if (thread)
            thread->waitUntilFinished();
    }
    
    void HighResTimer::start() {
        _run = true;
        Thread::runTask(this);
    }
    
    void HighResTimer::stop() {
        _run = false;
    }
    
    void HighResTimer::stopAndWait() {
        // Make a copy on stack to avoid a race condition between the if stament and the waitUntilFinished call
        Thread *thrd = thread; 
        
        _run = false;
        if (thrd)
            thrd->waitUntilFinished();
            
    }

    void HighResTimer::run() {
        thread = Thread::getThreadInstance();
        
        usleep(interval); // Delay first tick callback
        
        while (_run) {
            onTimerTick();
            usleep(interval);
        }
        
        thread = 0;
    }
};
