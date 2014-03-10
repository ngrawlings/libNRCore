//
//  Timer.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 10/03/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Timer__
#define __libNRCore__Timer__

#include "EventBase.h"

using namespace nrcore;

class Timer {
public:
    Timer(EventBase *event_base);
    virtual ~Timer();
    
    void start(int secs, int usecs);
    void stop();
    
protected:
    bool run;
    int secs, usecs;
    
    void schedule(long secs, int usecs);
    
    virtual void onTick() = 0;
    
private:
    EventBase *event_base;
    struct event *ev_schedule;
    
    static void ev_schedule_tick(int fd, short ev, void *arg);
};

#endif /* defined(__libNRCore__Timer__) */
