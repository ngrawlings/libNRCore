//
//  HighResTimer.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 16/06/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__HighResTimer__
#define __libNRCore__HighResTimer__

#include <libnrcore/types.h>

#include "Task.h"
#include "Thread.h"

namespace nrcore {

    class HighResTimer : Task {
    public:
        HighResTimer(useconds_t interval);
        virtual ~HighResTimer();
        
        void start();
        void stop();
        void stopAndWait();
        
    protected:
        void run();
        
        virtual void onTimerTick();
    
    private:
        bool _run;
        useconds_t interval;
        Thread *thread;
    };
    
};

#endif /* defined(__libNRCore__HighResTimer__) */
