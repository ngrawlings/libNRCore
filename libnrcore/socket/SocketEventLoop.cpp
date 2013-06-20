//
//  SocketEventLoop.cpp
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

#include "SocketEventLoop.h"

#include <signal.h>
#include <unistd.h>
#include <libnrcore/debug/Log.h>

namespace nrcore {

    SocketEventLoop::SocketEventLoop() : Task("Socket Event Loop") {
        event_signal = 0;
        ev_base = event_base_new();
    }

    SocketEventLoop::~SocketEventLoop() {
        event_base_free(ev_base);
    }

    void SocketEventLoop::run() {
        while (_run) {
            event_base_loop(ev_base, 4);
            usleep(10000);
        }
        logger.log("Event Loop Exiting");
    }

    void SocketEventLoop::runEventLoop(bool create_task, bool monitor_signals) {
        _run = true;
        
        if (monitor_signals && !event_signal) {
            event_signal = evsignal_new(ev_base, SIGHUP, signal_cb, this);
            event_base_set(ev_base, event_signal);
        }
        
        if (!create_task)
            run();
        else
            Thread::runTask(this);
    }

    void SocketEventLoop::breakEventLoop() {
        _run = false;
        event_base_loopbreak(ev_base);
    }

    event_base* SocketEventLoop::getEventBase() {
        return ev_base;
    }

    void SocketEventLoop::signal_cb(evutil_socket_t fd, short what, void *data) {
        reinterpret_cast<SocketEventLoop*>(data)->breakEventLoop();
    }
    
}