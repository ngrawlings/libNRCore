//
//  Socket.cpp
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

#include "Socket.h"
#include <libnrcore/debug/Log.h>

#ifdef _win32

#include <windows.h>
#include <winsock.h>

#else

#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <sys/socket.h>

#endif

namespace nrcore {

    unsigned long Socket::descriptor_count = 0;
    DescriptorInstanceMap<Socket*> *Socket::descriptors = 0;
    LinkedList<struct event *> *Socket::event_release_queue;

    Socket::Socket(event_base *ev_base, int _fd) : Task("Socket"), recv_lock("recv_lock"), send_lock("send_lock"), operation_lock("operation_lock"), transmitter(this), state(OPEN) {
        this->ev_base = ev_base;
        fd = _fd;

        struct stat statbuf;
        fstat(fd, &statbuf);
        
        if (!S_ISSOCK(statbuf.st_mode))
            throw "Invalid Fd";
        
        descriptors->lock.lock();
        
        if (descriptors->isActive(fd))
            descriptors->get(fd)->release();
        
        descriptors->set(fd, this);
        descriptors->setActive(fd, true);
        
        descriptors->lock.release();
        
        event_write = event_read = 0;

        output_buffer = evbuffer_new();
    }

    Socket::~Socket() {
        descriptors->lock.lock();
        
        if (descriptors->equals(fd, this)) {
            descriptors->setActive(fd, false);
            descriptors->set(fd, 0);
        }
        
        descriptors->lock.release();
        
        close();

        logger.log(Log::LOGLEVEL_NOTICE, "Client Connection Destroyed -> fd %d", fd);
    }

    void Socket::enableEvents() {
        unsigned long long fd_num = fd;
        event_write = event_new(ev_base, fd, EV_WRITE, ev_write, (void*)fd_num);
        event_read = event_new(ev_base, fd, EV_READ|EV_PERSIST, ev_read, (void*)fd_num);
        event_add(event_read, NULL);
    }

    void Socket::run() {
        ssize_t read;
        
        recv_lock.setBusy();
        
        if (state != OPEN) {
            release();
            if (recv_lock.isLockedByMe())
                recv_lock.release();
            return;
        }

        read = ::recv(fd, recv_buf, 256, 0);
        while (read > 0) {
            try {
                received(recv_buf, (int)read);
            } catch (...) {
                logger.log(Log::LOGLEVEL_ERROR, "Error with in receive event");
            }
            read = ::recv(fd, recv_buf, 256, 0);
        }
        
        if ((read == -1 && errno != EAGAIN) || read == 0) {
            if (read == -1)
                logger.log(Log::LOGLEVEL_NOTICE, "Socket Error: fd %d, recv result %d, errno %d", fd, read, errno);
            else
                logger.log(Log::LOGLEVEL_NOTICE, "Socket Closed By Client: fd %d", fd);
            
            release();
        }
        
        recv_lock.setWaiting();
        
        descriptors->finished(fd);
    }

    void Socket::recv() {
        ev_read(fd, 0, 0);
    }

    int Socket::send(char *bytes, int len) {
        int sent = 0, s;
        int flags = 0;
        
    #ifdef MSG_NOSIGNAL
        flags = MSG_NOSIGNAL;
    #endif
        
        try {
            
            send_lock.lock();
            
            if (state == OPEN) {
                if (evbuffer_get_length(output_buffer)) {
                    
                    evbuffer_add(output_buffer, bytes, len);
                    if (!event_pending(event_write, EV_READ, NULL))
                        event_add(event_write, NULL);
                    
                    if (send_lock.isLockedByMe())
                        send_lock.release();
                    
                    flush();
                    
                    sent = 0;
                    
                } else {
                
                    while (sent < len) {
                        s = (int)::send(fd, bytes, (ssize_t)len, flags);
                        if (s==-1 && errno==EAGAIN) {
                            evbuffer_add(output_buffer, &bytes[sent], len-sent);
                            if (!event_pending(event_write, EV_READ, NULL))
                                event_add(event_write, NULL);
                            
                            if (send_lock.isLockedByMe())
                                send_lock.release();
                            
                            flush();
                            break;
                        } else if (s==-1) {
                            logger.log(Log::LOGLEVEL_NOTICE, "Socket Error: %d", errno);
                            close();
                            sent = -1;
                            
                            if (send_lock.isLockedByMe())
                                send_lock.release();
                            
                            break;
                        } else {
                            sent += s;
                            
                            send_lock.release();
                        
                        }
                    }
                }
            } else {
                if (send_lock.isLockedByMe())
                    send_lock.release();
            }
            
        } catch (...) { // Catch mutex failiure
            if (send_lock.isLockedByMe())
                send_lock.release();
        }
        
        return sent;
    }

    void Socket::close() {
        operation_lock.lock();
        
        try {
            Task::removeTasks(this);
            
            if (state != CLOSED) {
                state = CLOSED;
                ::close(fd);
                logger.log(Log::LOGLEVEL_NOTICE, "Client Disconnected -> fd %d", fd);
                disconnected();
            } else {
                // This is purely to report a situation that should never occure, but I think there is a race condition that leads to this state
                // by reporting the socket state when this occures will provide an insight to help avoid fd escalations.
                int res = 0;
                socklen_t sock_len = sizeof(res);
                getsockopt(fd, SOL_SOCKET, SO_ERROR, &res, &sock_len);
                logger.log(Log::LOGLEVEL_ERROR, "Socket Already Closed -> fd %d with errno %d", fd, res);
            }

            if ( (event_read && event_pending(event_read, EV_READ, 0)) || (event_write && event_pending(event_write, EV_WRITE, 0)) ) {
                event_active(event_read, 0, 0);
                event_active(event_write, 0, 0);
            }
            
            if (output_buffer) {
                evbuffer_free(output_buffer);
                output_buffer = 0;
            }
        } catch (...) {
            logger.log(Log::LOGLEVEL_NOTICE, "Socket Close - Unknown Exception");
        }
        
        operation_lock.release();
    }

    void Socket::release() {
        descriptors->lock.lock();
        
        operation_lock.lock();

        if (state != CLOSED)
            close();
        
        if (descriptors->isActive(fd) && descriptors->equals(fd, this))
            descriptors->setActive(fd, false);
        
        SocketDestroy *sd = new SocketDestroy(this);
        Task::queueTask(sd);
            
        operation_lock.release();
        
        descriptors->lock.release();
    }

    size_t Socket::getTransmitionQueueSize() {
        size_t ret = 0;
        send_lock.lock();
        ret = evbuffer_get_length(output_buffer);
        send_lock.release();
        return ret;
    }

    void Socket::flush() {
        if ( state == OPEN )
            transmitter.runNow();
    }

    void Socket::ev_read(int fd, short ev, void *arg) {
        descriptors->lock.lock();
        
        if (descriptors->isActive(fd)) {
            
            Socket *client;
            
            try {
                if ((client = descriptors->getAndLock(fd))) {
                    
                    if (client->recv_lock.getState() == TaskMutex::WAITING && client->operation_lock.tryLock()) {
                        
                        if ( client->state == OPEN ) {
                            
                            client->recv_lock.lock();
                                
                            client->recv_lock.setQueued();
                            Thread::runTask(client);
                                
                            client->recv_lock.release();
     
                        } else {
                            client->release();
                        }
                    
                    } else
                        descriptors->finished(fd);
                    
                }
            } catch(...) {
                logger.log(Log::LOGLEVEL_ERROR, "Recv Invalid Socket");
            }
            
            try {
                if (client && client->operation_lock.isLockedByMe())
                    client->operation_lock.release();
            } catch(...) {}
        }
        
        // The queue must be released within the base event thread
        while(event_release_queue->length()) {
            struct event* ev = event_release_queue->get(0);
            event_release_queue->remove(0);
            
            event_del(ev);
            event_free(ev);
        }
        
        descriptors->lock.release();
    }

    void Socket::ev_write(int fd, short ev, void *arg) {
        if (descriptors->isActive(fd)) {
            try {
                
                Socket *client = descriptors->get(fd);
                
                if (!client)
                    return;
                
                if ( client->state == OPEN)
                    Thread::runTask(&client->transmitter);
                    
                else if ( client->state != OPEN )
                    client->release();
                
            } catch (...) {
            }
        }
    }

    int Socket::connect(const char* addr, unsigned short port) {
        unsigned char addr_bytes[sizeof(struct in6_addr)];
        int af_type = 0;
        sockaddr_in *saddr = (sockaddr_in*)addr_bytes;
        sockaddr_in6 *saddr6 = (sockaddr_in6*)addr_bytes;
        
        memset(addr_bytes, 0, sizeof(struct in6_addr));
        
        if (inet_pton(AF_INET, addr, &saddr->sin_addr)==1) {
            af_type = AF_INET;
        } else if (inet_pton(AF_INET6, addr, &saddr6->sin6_addr)==1) {
            af_type = AF_INET6;
        } else {
            struct hostent *hostEntry;
            hostEntry = gethostbyname(addr);
            if (hostEntry->h_addrtype == AF_INET) {
                af_type = AF_INET;
                memcpy(&saddr->sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);
            } else if (hostEntry->h_addrtype == AF_INET6) {
                af_type = AF_INET6;
                memcpy(&saddr6->sin6_addr, hostEntry->h_addr_list[0], hostEntry->h_length);
            }
        }
        
        if (af_type == AF_INET) {
            sockaddr_in *saddr = (sockaddr_in*)addr_bytes;
            saddr->sin_family = AF_INET;
            saddr->sin_port = htons(port);
        } else if (af_type == AF_INET6) {
            sockaddr_in6 *saddr = (sockaddr_in6*)addr_bytes;
            saddr->sin6_family = AF_INET6;
            saddr->sin6_port = htons(port);
        }
        
        int sock_fd = socket(af_type, SOCK_STREAM, IPPROTO_TCP);
        if (!::connect(sock_fd, (sockaddr*)addr_bytes, af_type == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6))) {
            setNonBlocking(sock_fd);
            logger.log(Log::LOGLEVEL_NOTICE, "Outbound connection connected -> %d\r\n", sock_fd);
            return sock_fd;
        }
        
        logger.log(Log::LOGLEVEL_NOTICE, "Failed to connect, errno %d", errno);

        return 0;
    }

    int Socket::setNonBlocking(int fd) {
        int flags;
        
        flags = fcntl(fd, F_GETFL);
        if (flags < 0)
            return flags;
        flags |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) < 0)
            return -1;
        
        return 0;
    }

    void Socket::initSocketSubSystem() {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(1, 1), &wsaData);
#endif
        descriptors = new DescriptorInstanceMap<Socket*>();
        event_release_queue = new LinkedList<struct event*>();
    }

    void Socket::releaseSocketSubsystem() {
        for (unsigned int i=0; i<descriptor_count; i++)
            if (!descriptors->isActive(i))
                descriptors->get(i)->release();
        
        delete descriptors;
        delete event_release_queue;
        
#ifdef _WIN32
        WSACleanup();
#endif
    }

    unsigned int Socket::getSocketCount() {
        return descriptors->getCount();
    }

    Socket* Socket::getSocketByDescriptor(unsigned long fd) {
        if (descriptors->isActive(fd))
            return descriptors->get(fd);
        return 0;
    }
    
}
