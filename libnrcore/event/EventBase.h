//
//  EventBase.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 10/03/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__EventBase__
#define __libNRCore__EventBase__

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>
#include <event2/thread.h>

#include <libnrcore/threading/Task.h>
#include <libnrcore/threading/Thread.h>

namespace nrcore {
    
    class EventBase : public Task {
    public:
        EventBase();
        virtual ~EventBase();
        
        void runEventLoop(bool create_task=false);
        void breakEventLoop();
        
        event_base* getEventBase();
        
    protected:
        virtual void run();
        
    private:
        struct event_base *ev_base;
        struct event *event_signal;
        struct event *ev_schedule;
        
        bool _run;
    };
    
}

#endif /* defined(__libNRCore__EventBase__) */
