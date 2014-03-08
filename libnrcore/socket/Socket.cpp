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

#include <libnrcore/types.h>
#include <libnrcore/debug/Log.h>

#ifdef WIN32

#include <windows.h>
#include <winsock.h>

#else

#include <sys/mman.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <sys/socket.h>

#endif

namespace nrcore {

    DescriptorInstanceMap<Socket*> *Socket::descriptors;
    Mutex *Socket::descriptors_lock;
    
    LinkedList<Socket*> *Socket::socket_release_queue;
    Mutex *Socket::release_lock;
    
    LinkedList<Socket*> *Socket::sockets;

    Socket::Socket(event_base *ev_base, int _fd) : Task("Socket"), recv_lock("recv_lock"), send_lock("send_lock"), operation_lock("operation_lock"), transmitter(this), state(OPEN) {
        this->ev_base = ev_base;
        fd = _fd;

        struct stat statbuf;
        fstat(fd, &statbuf);
        
        if (!S_ISSOCK(statbuf.st_mode))
            throw "Invalid Fd";
        
        event_write = event_read = 0;

        output_buffer = evbuffer_new();
        
        {
            descriptors_lock->lock();
            if (descriptors->get(fd) != 0) {
                descriptors_lock->release();
                descriptors->get(fd)->disconnected();
            }
            if (!descriptors_lock->isLocked())
                descriptors_lock->lock();
            
            descriptors->set(fd, this);
            descriptors_lock->release();
        }
        
        sockets->add(this);
    }

    Socket::~Socket() {
        if (fd)
            close();
        
        if (event_write) {
            event_del(event_write);
            event_free(event_write);
        }
        
        if (event_read) {
            event_del(event_read);
            event_free(event_read);
        }
        
        if (output_buffer)
            evbuffer_free(output_buffer);
        
        sockets->remove(this);
        
        LOG(Log::LOGLEVEL_NOTICE, "Socket Destroyed -> fd %d, ptr: %p", fd, this);
    }

    void Socket::enableEvents() {
        event_write = event_new(ev_base, fd, EV_WRITE, ev_write, (void*)this);
        event_read = event_new(ev_base, fd, EV_READ|EV_PERSIST, ev_read, (void*)this);
        event_add(event_read, NULL);
    }

    void Socket::run() {
        ssize_t read;
        
        recv_lock.setBusy();
        
        if (state != OPEN) {
            if (recv_lock.isLockedByMe())
                recv_lock.release();
            return;
        }

        read = ::recv(fd, recv_buf, 256, 0);
        while (read > 0) {
            try {
                received(recv_buf, (int)read);
            } catch (...) {
                LOG(Log::LOGLEVEL_ERROR, "Error with in receive event");
            }
            read = ::recv(fd, recv_buf, 256, 0);
        }
        
        if ((read == -1 && errno != EAGAIN) || read == 0) {
            if (read == -1)
                LOG(Log::LOGLEVEL_NOTICE, "Socket Error: fd %d, recv result %d, errno %d", fd, read, errno);
            else
                LOG(Log::LOGLEVEL_NOTICE, "Socket Closed By Client: fd %d", fd);
            
            close();
        }
        
        recv_lock.setWaiting();
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
                            LOG(Log::LOGLEVEL_NOTICE, "Socket Error: %d", errno);
                            close();
                            sent = -1;
                            
                            if (send_lock.isLockedByMe())
                                send_lock.release();
                            
                            break;
                        } else
                            sent += s;
                    }
                    
                }
            }
            
            if (send_lock.isLockedByMe())
                send_lock.release();
            
        } catch (...) { // Catch mutex failiure
            if (send_lock.isLockedByMe())
                send_lock.release();
        }
        
        return sent;
    }

    void Socket::poll() {
        ev_read(fd, 0, this);
    }
    
    void Socket::close() {
        if (!fd)
            return;
        
        operation_lock.lock();
        
        descriptors_lock->lock();
        if (descriptors->get(fd) != this) {
            operation_lock.release();
            descriptors_lock->release();
            return;
        }
            
        descriptors->set(fd, 0);
        descriptors_lock->release();
        
        try {
            Task::removeTasks(this);
            
            if (state != CLOSED) {
                state = CLOSED;
                ::close(fd);
                fd = 0;
                LOG(Log::LOGLEVEL_NOTICE, "Socket Disconnected -> fd %d", fd);
                disconnected();
            }

            if ( (event_read && event_pending(event_read, EV_READ, 0)) || (event_write && event_pending(event_write, EV_WRITE, 0)) ) {
                event_active(event_read, 0, 0);
                event_active(event_write, 0, 0);
            }
            
        } catch (...) {
            LOG(Log::LOGLEVEL_NOTICE, "Socket Close - Unknown Exception");
        }
        
        operation_lock.release();
    }
    
    void Socket::shutdown() {
        ::shutdown(fd, SHUT_WR);
    }
    
    void Socket::setState(STATE state) {
        this->state = state;
    }

    void Socket::release() {
        release_lock->lock();

        if (state != RELEASED) {
            if (state != CLOSED)
                close();
        
            state = RELEASED;
            socket_release_queue->remove(this); // remove if it is already in queue and add again, simple one liner to avoid duplicates
            socket_release_queue->add(this);
            
            LOG(Log::LOGLEVEL_NOTICE, "Socket Released: fd %d", fd);
        } else
            LOG(Log::LOGLEVEL_ERROR, "Socket Already Released: fd %d", fd);
        
        release_lock->release();
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
        Socket *client = reinterpret_cast<Socket*>(arg);
            
        if (client->recv_lock.getState() == TaskMutex::WAITING && client->operation_lock.tryLock()) {

            if ( client->state == OPEN ) {                
                if (client->recv_lock.tryLock()) {
                    if (client->recv_lock.getState() == TaskMutex::WAITING) {
                    
                        client->recv_lock.setQueued();
                        Thread::runTask(client);
                        
                    }
                    client->recv_lock.release();
                }
            }

        }
            
        try {
            if (client && client->operation_lock.isLockedByMe())
                client->operation_lock.release();
        } catch(...) {}
    }

    void Socket::ev_write(int fd, short ev, void *arg) {
        try {
            Socket *client = reinterpret_cast<Socket*>(arg);
                
            if ( client->state == OPEN)
                Thread::runTask(&client->transmitter);
                
        } catch (...) {
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
            LOG(Log::LOGLEVEL_NOTICE, "Outbound connection connected -> %d\r\n", sock_fd);
            return sock_fd;
        }
        
        LOG(Log::LOGLEVEL_NOTICE, "Failed to connect, errno %d", errno);

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
        descriptors_lock = new Mutex();
        socket_release_queue = new LinkedList<Socket*>();
        release_lock = new Mutex();
        
        sockets = new LinkedList<Socket*>();
    }

    void Socket::releaseSocketSubsystem() {
        delete socket_release_queue;
        delete release_lock;
        delete descriptors_lock;
        delete descriptors;
        
        delete sockets;
        
#ifdef _WIN32
        WSACleanup();
#endif
    }
    
    LinkedList<Socket*> Socket::getOpenSockets() {
        return *sockets;
    }
    
    void Socket::processReleaseSocketQueue() {
        if (release_lock->tryLock()) {
            while(socket_release_queue->length()) {
                LOG(Log::LOGLEVEL_NOTICE, "Destroying Socket -> fd %d, ptr: %p", socket);
                
                Socket* socket = socket_release_queue->get(0);
                
                descriptors_lock->lock();
                
                int fd = socket->getDescriptorNumber();
                if (socket == descriptors->get(fd)) {
                    if (!socket->recv_lock.tryLock())
                        socket->recv_lock.waitUntilFinished();
                }
                
                descriptors_lock->release();
                
                delete socket; 
                
                socket_release_queue->remove(socket);
            }
            release_lock->release();
        }
    }
    
    unsigned short Socket::getRemotePort() {
        struct sockaddr_in sock_addr;
        socklen_t sock_len = sizeof(sock_addr);
        
        getpeername(fd, (struct sockaddr*)&sock_addr, &sock_len);
        
        return htons(sock_addr.sin_port);
    }
    
    unsigned short Socket::getLocalPort() {
        struct sockaddr_in sock_addr;
        socklen_t sock_len = sizeof(sock_addr);
        
        getsockname(fd, (struct sockaddr*)&sock_addr, &sock_len);
        
        return htons(sock_addr.sin_port);
    }
    
    String Socket::getRemoteAddress() {
        struct sockaddr_in6 sock_addr;
        socklen_t sock_len = sizeof(sock_addr);
        
        getpeername(fd, (struct sockaddr*)&sock_addr, &sock_len);
        
        char buffer[INET6_ADDRSTRLEN];
        getnameinfo((struct sockaddr*)&sock_addr, sock_len, buffer, sizeof(buffer), 0, 0, NI_NUMERICHOST);
        return String(buffer);
    }
    
    String Socket::getLocalAddress() {
        struct sockaddr_in6 sock_addr;
        socklen_t sock_len = sizeof(sock_addr);
        
        getsockname(fd, (struct sockaddr*)&sock_addr, &sock_len);
        
        char buffer[INET6_ADDRSTRLEN];
        getnameinfo((struct sockaddr*)&sock_addr, sock_len, buffer, sizeof(buffer), 0, 0, NI_NUMERICHOST);
        return String(buffer);
    }
    
}
