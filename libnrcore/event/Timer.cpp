//
//  Timer.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 10/03/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "Timer.h"

Timer::Timer(EventBase *event_base) {
    this->event_base = event_base;
    ev_schedule = evtimer_new(event_base->getEventBase(), ev_schedule_tick, this);
    run = false;
}

Timer::~Timer() {
    event_del(ev_schedule);
    event_free(ev_schedule);
}

void Timer::start(int secs, int usecs) {
    this->secs = secs;
    this->usecs = usecs;
    run = true;
    schedule(secs, usecs);
}

void Timer::stop() {
    run = false;
}

void Timer::schedule(long secs, int usecs) {
    struct timeval tv;
    
    tv.tv_sec = secs;
    tv.tv_usec = usecs;
    
    evtimer_add(ev_schedule, &tv);
}

void Timer::ev_schedule_tick(int fd, short ev, void *arg) {
    Timer *timer = reinterpret_cast<Timer*>(arg);
    if (timer->run) {
        timer->onTick();
        if (timer->run)
            timer->schedule(timer->secs, timer->usecs);
    }
}