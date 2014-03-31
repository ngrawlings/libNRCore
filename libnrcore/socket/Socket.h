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

#include <libnrcore/event/EventBase.h>

#include <libnrcore/io/Stream.h>
#include <libnrcore/memory/Memory.h>
#include <libnrcore/memory/StaticArray.h>
#include <libnrcore/memory/DescriptorInstanceMap.h>
#include <libnrcore/threading/Thread.h>
#include <libnrcore/debug/Log.h>

#include <libnrcore/threading/TaskMutex.h>

#include "Address.h"

namespace nrcore {

    class Socket : public Stream {
    public:        
        enum STATE {
            OPEN,
            CLOSED,
            RELEASED
        };
        
    public:
        Socket(EventBase *event_base, const char* addr);
        Socket(EventBase *event_base, const char* addr, unsigned short port);
        Socket(EventBase *event_base, int _fd);
        
        virtual ~Socket();
        
        ssize_t write(const char* buf, size_t sz);
        ssize_t read(char* buf, size_t sz);
        
        virtual int send(const char *bytes, const int len);
        void poll();
        void close();
        void shutdown();
        
        void release();
        
        size_t getTransmitionQueueSize();
        void flush();
        
        static int connect(Address &address, unsigned short port);
        static int connect(const char* addr, unsigned short port);
        
        static void initSocketSubSystem();
        static void releaseSocketSubsystem();
        
        static LinkedList<Socket*> getOpenSockets();
        static void closeAllSockets();
        
        static Mutex *getReleaseLock() { return release_lock; }

        int getDescriptorNumber() { return fd; }
        STATE getState() { return state; }
        
        unsigned short getRemotePort();
        unsigned short getLocalPort();
        
        String getRemoteAddress();
        String getLocalAddress();
        
        void setReceiveBufferSize(size_t size);
        
    protected:
        
        class ReceiveTask : public Task {
        public:
            friend class Socket;
            
            ReceiveTask(Socket *socket) : recv_lock("recv_lock") {
                this->socket = socket;
                buf_sz = 256;
                this->recv_buf = new char[buf_sz];
            }
            
            virtual ~ReceiveTask() {
                delete recv_buf;
            }
            
            void setBufferSize(size_t size) {
                recv_lock.lock();
                delete recv_buf;
                buf_sz = size;
                recv_buf = new char[size];
                recv_lock.release();
            }
            
        protected:
            void run();
            
            Socket *socket;
            TaskMutex recv_lock;
            char *recv_buf;
            size_t buf_sz;
        };
        
        class TransmissionTask : public Task {
        public:
            TransmissionTask(Socket *socket) {
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
            Socket *socket;
        };
        
        
        Mutex send_lock;
        Mutex operation_lock;
        static Mutex *release_lock;
        
        STATE state;
        
        virtual bool beforeReceived(const char *bytes, const int len) { return true; }
        virtual void received(const char *bytes, const int len) = 0;
        virtual void disconnected() {};
        virtual void onDestroy() {};
        
        ReceiveTask         receiver;
        TransmissionTask    transmission;
        
        void setState(STATE state);

    private:
        
        class SocketDestroy : public Task {
        public:
            SocketDestroy(Socket *socket) {
                this->socket = socket;
            }
            
            virtual ~SocketDestroy() {}
            
        protected:
            void run() {
                Socket::getReleaseLock()->lock();
                logger.log(Log::LOGLEVEL_NOTICE, "SocketDestroy: %p %d", socket, socket->getDescriptorNumber());
                delete socket;
                Socket::getReleaseLock()->release();
                finished();
            }
            
        private:
            Socket *socket;
        };
        
        static DescriptorInstanceMap<Socket*> *descriptors;
        static LinkedList<Socket*> *sockets;
        
        struct event    *event_read, *event_write;
        struct evbuffer *output_buffer;
        
        EventBase *event_base;
        
        static Mutex *descriptors_lock;
        
        void enableEvents();
        
        static void ev_read(int fd, short ev, void *arg);
        static void ev_write(int fd, short ev, void *arg);
        
        static int setNonBlocking(int fd);
        
        void init();
    };
    
};

#endif /* defined(__PeerConnector__Socket__) */
