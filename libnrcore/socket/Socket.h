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
#include <libnrcore/memory/DescriptorInstanceMap.h>

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
                logger.log("Transmit queue: %d", buf_len);
                    
                if (buf_len) {
                    
                    evbuffer_write(socket->output_buffer, socket->fd);
                    if ( evbuffer_get_length(socket->output_buffer) && !event_pending(socket->event_write, EV_READ, NULL))
                        event_add(socket->event_write, NULL);
                    
                }
                
            } catch (...) {
                logger.log("Error in Socket Transmitter");
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
        friend class SocketDestroy;
        
        enum STATE {
            OPEN,
            CLOSED
        };
        
    public:
        Socket(event_base *ev_base, int _fd);
        virtual ~Socket();
        
        void enableEvents();
        
        void recv();
        int send(char *bytes, int len);
        void close();
        
        void release();
        
        size_t getTransmitionQueueSize();
        void flush();
        
        static int connect(const char* addr, unsigned short port);
        
        static void initSocketSubsystem();
        static void releaseSocketSubsystem();

        int getDescriptorNumber() { return fd; }
        STATE getState() { return state; }
        
        static unsigned int getSocketCount();
        static Socket* getSocketByDescriptor(unsigned long fd);
        
    protected:
        TaskMutex recv_lock;
        Mutex send_lock;
        Mutex operation_lock;
        
        static unsigned long descriptor_count;
        static DescriptorInstanceMap<Socket*> *descriptors;
        //static StaticArray<Socket*, MAX_SOCKET_DESCRIPTORS> descritptors;
        
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
        
        STATE state;
        
        char recv_buf[256];
        
        static void ev_read(int fd, short ev, void *arg);
        static void ev_write(int fd, short ev, void *arg);
        
        static int setNonBlocking(int fd);
    };

    class SocketDestroy : public Task {
    public:
        SocketDestroy(Socket *socket) : Task("Socket Destroy") {
            this->socket = socket;
        }
        virtual ~SocketDestroy() {}
        
    protected:
        void run() {
            try {
                if (socket->recv_lock.getState() != TaskMutex::WAITING)
                    socket->recv_lock.waitUntilFinished();
                
                socket->send_lock.lock();

                socket->onDestroy();
                
                event_free(socket->event_read);
                event_free(socket->event_write);
                
                socket->event_read = 0;
                socket->event_write = 0;
                
                delete socket;
                
                logger.log("released socket");
            } catch (...) {
                logger.log("Socket Destory unknown error");
            }

            delete this;
        }
        
    private:
        Socket *socket;
    };
    
};

#endif /* defined(__PeerConnector__Socket__) */
