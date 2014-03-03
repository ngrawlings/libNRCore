//
//  Socket.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 31/12/2012.
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

#ifndef __PeerConnector__Socket__
#define __PeerConnector__Socket__

#include <errno.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>

#include <libnrcore/memory/Ref.h>
#include <libnrcore/memory/Memory.h>
#include <libnrcore/memory/StaticArray.h>
#include <libnrcore/threading/Thread.h>
#include <libnrcore/debug/Log.h>

#include <libnrcore/threading/TaskMutex.h>

namespace nrcore {

    template <class T>
    class SocketTransmitter : public Task {
    public:
        SocketTransmitter<T>(T socket) : Task("SocketTransmitter") {
            this->socket = socket;
        }
        
        void runNow() {
            run();
        }

    protected:
        void run() {
            socket->send_lock.lock();
            try {
                size_t buf_len = evbuffer_get_length(socket->output_buffer);
                LOG(Log::LOGLEVEL_NOTICE, "Transmit queue: %d", buf_len);
                    
                if (buf_len) {
                    
                    evbuffer_write(socket->output_buffer, socket->fd);
                    if ( evbuffer_get_length(socket->output_buffer) && !event_pending(socket->event_write, EV_READ, NULL))
                        event_add(socket->event_write, NULL);
                    
                }
                
            } catch (...) {
                LOG(Log::LOGLEVEL_ERROR, "Error in Socket Transmitter", 0);
            }
            if (socket->send_lock.isLockedByMe())
                socket->send_lock.release();
        }
        
    private:
        T socket;
    };

    class Socket : public Task {
    public:
        friend class SocketTransmitter<Socket*>;
        
        enum STATE {
            OPEN,
            CLOSED,
            RELEASED
        };
        
    public:
        Socket(event_base *ev_base, int _fd);
        virtual ~Socket();
        
        void enableEvents();
        
        int send(char *bytes, int len);
        void close();
        void shutdown();
        
        void release();
        
        size_t getTransmitionQueueSize();
        void flush();
        
        static int connect(const char* addr, unsigned short port);
        
        static void initSocketSubSystem();
        static void releaseSocketSubsystem();

        int getDescriptorNumber() { return fd; }
        STATE getState() { return state; }
        
        static void processReleaseSocketQueue();
        
    protected:
        TaskMutex recv_lock;
        Mutex send_lock;
        Mutex operation_lock;
        
        static LinkedList<Socket*> *socket_release_queue;

        // Task entry
        void run();
        
        virtual void received(char *bytes, int len) = 0;
        virtual void disconnected() {};
        virtual void onDestroy() {};
        
        SocketTransmitter<Socket*> transmitter;

    private:
        static int max_fds;
        
        int fd;
        struct event    *event_read, *event_write;
        struct evbuffer *output_buffer;
        
        struct event_base *ev_base;
        
        static Mutex *release_lock;
        
        STATE state;
        
        char recv_buf[256];
        
        static void ev_read(int fd, short ev, void *arg);
        static void ev_write(int fd, short ev, void *arg);
        
        static int setNonBlocking(int fd);
    };
    
};

#endif /* defined(__PeerConnector__Socket__) */
