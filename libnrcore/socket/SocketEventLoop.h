//
//  SocketEventLoop.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 07/03/2012.
//  Copyright (c) 2012. All rights reserved.
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

#ifndef SOCKETEVENTLOOP_H_
#define SOCKETEVENTLOOP_H_

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>

#include <libnrcore/threading/Task.h>
#include <libnrcore/threading/Thread.h>

namespace nrcore {

    class SocketEventLoop : public Task {
    public:
        SocketEventLoop();
        virtual ~SocketEventLoop();

        void runEventLoop(bool create_task, bool monitor_signals=false);
        void breakEventLoop();

        event_base* getEventBase();

    protected:
        virtual void run();

    private:
        struct event_base *ev_base;
        struct event *event_signal;
        struct event *ev_schedule;
        
        bool _run;
        
        static void signal_cb(evutil_socket_t fd, short what, void *data);
        static void ev_schedule_tick(int fd, short ev, void *arg);
    };
    
}

#endif /* SOCKETEVENTLOOP_H_ */
