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
#include <libnrcore/memory/String.h>
#include <libnrcore/memory/StringList.h>

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

#endif

namespace nrcore {

    DescriptorInstanceMap<Socket*> *Socket::descriptors;
    Mutex *Socket::descriptors_lock;
    Mutex *Socket::release_lock;
    
    LinkedList<Socket*> *Socket::sockets;
    
    Socket::Socket(EventBase *event_base, const char* addr) : Stream(0),
                event_base(event_base),
                send_lock(CONST("send_lock")),
                operation_lock(CONST("operation_lock")),
                receiver(this),
                transmission(this),
                state(OPEN)
    {
        String address(addr);
        unsigned short port;
        String ip = address.extract("[", "]");
        if (ip.length() == 0) {
            StringList parts(address, ":");
            port = atoi(parts[1]);
            address = parts[0];
        } else {
            port = atoi(address.substr((int)ip.length()+3));
            address = ip;
        }
        
        fd = connect(address, port);
        if (fd)
            init();
        else
            state = CLOSED;
    }
    
    Socket::Socket(EventBase *event_base, const char* addr, unsigned short port) : Stream(0),
                event_base(event_base),
                send_lock(CONST("send_lock")),
                operation_lock(CONST("operation_lock")),
                receiver(this),
                transmission(this),
                state(OPEN)
    {
        fd = connect(addr, port);
        if (fd)
            init();
        else
            state = CLOSED;
    }

    Socket::Socket(EventBase *event_base, int _fd) : Stream(_fd),
                event_base(event_base),
                send_lock(CONST("send_lock")),
                operation_lock(CONST("operation_lock")),
                receiver(this),
                transmission(this),
                state(OPEN)
    {
        init();
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
    
    void Socket::init() {
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
        
        enableEvents();
    }

    void Socket::enableEvents() {
        event_write = event_new(event_base->getEventBase(), fd, EV_WRITE, ev_write, (void*)this);
        event_read = event_new(event_base->getEventBase(), fd, EV_READ|EV_PERSIST, ev_read, (void*)this);
        event_add(event_read, NULL);
    }

    void Socket::ReceiveTask::run() {
        ssize_t read;
 
        recv_lock.setBusy();
        
        if (socket->state != OPEN) {
            if (recv_lock.isLockedByMe())
                recv_lock.release();
            return;
        }

        read = ::recv(socket->fd, recv_buf, buf_sz, 0);
        while (read > 0) {
            try {
                if (socket->beforeReceived(recv_buf, (int)read))
                    socket->received(recv_buf, (int)read);
            } catch (...) {
                LOG(Log::LOGLEVEL_ERROR, "Error with in receive event");
            }
            read = ::recv(socket->fd, recv_buf, 256, 0);
        }
        
        if ((read == -1 && errno != EAGAIN) || read == 0) {
            if (read == -1)
                LOG(Log::LOGLEVEL_NOTICE, "Socket Error: fd %d, recv result %d, errno %d", socket->fd, read, errno);
            else
                LOG(Log::LOGLEVEL_NOTICE, "Socket Closed By Client: fd %d", socket->fd);
            
            socket->close();
        }
        
        recv_lock.setWaiting();

    }
    
    ssize_t Socket::write(const char* buf, size_t sz) {
        return send(buf, (int)sz);
    }
    
    ssize_t Socket::read(char* buf, size_t sz) {
        return ::recv(fd, buf, sz, 0);
    }

    int Socket::send(const char *bytes, const int len) {
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
        operation_lock.lock();
        
        if (!fd) {
            operation_lock.release();
            return;
        }
        
        descriptors_lock->lock();
        if (descriptors->get(fd) != this) {
            operation_lock.release();
            descriptors_lock->release();
            return;
        }
            
        descriptors->set(fd, 0);
        descriptors_lock->release();
        
        try {
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
        operation_lock.lock();
        
        if (state != RELEASED) {
            if (state != CLOSED)
                close();
        
            state = RELEASED;
            
            LOG(Log::LOGLEVEL_NOTICE, "Socket Released: %p", this);
            
            SocketDestroy *sock_destroy = new SocketDestroy(this);
            Thread* thread = receiver.getAquiredThread();
            if (thread)
                thread->queueTaskToCurrentThread(sock_destroy);
            else
                Task::queueTask(sock_destroy);
            
        } else
            LOG(Log::LOGLEVEL_ERROR, "Socket Already Released: fd %d", fd);
        
        operation_lock.release();
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
            transmission.runNow();
    }

    void Socket::ev_read(int fd, short ev, void *arg) {
        Socket *client = reinterpret_cast<Socket*>(arg);
            
        if (client->receiver.recv_lock.getState() == TaskMutex::WAITING && client->operation_lock.tryLock()) {

            if ( client->state == OPEN ) {                
                if (client->receiver.recv_lock.tryLock()) {
                    if (client->receiver.recv_lock.getState() == TaskMutex::WAITING) {
                    
                        client->receiver.recv_lock.setQueued();
                        Thread::runTask(&client->receiver);
                        
                    }
                    client->receiver.recv_lock.release();
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
                Thread::runTask(&client->transmission);
                
        } catch (...) {
        }
    }
    
    int Socket::connect(Address &address, unsigned short port) {
        unsigned char addr_bytes[sizeof(struct sockaddr_in6)];
        Address::ADDRESS_TYPE type = address.getType();
        
        memset(addr_bytes, 0, sizeof(struct sockaddr_in6));
        
        if (type == Address::IPV4) {
            sockaddr_in *saddr = (sockaddr_in*)addr_bytes;
            saddr->sin_family = AF_INET;
            saddr->sin_port = htons(port);
            memcpy(&saddr->sin_addr, address.getAddr(), sizeof(struct in_addr));
        } else if (type == Address::IPV6) {
            sockaddr_in6 *saddr = (sockaddr_in6*)addr_bytes;
            saddr->sin6_family = AF_INET6;
            saddr->sin6_port = htons(port);
            memcpy(&saddr->sin6_addr, address.getAddr(), sizeof(struct in6_addr));
        }
        
        int sock_fd = socket((type==Address::IPV4 ? AF_INET : AF_INET6), SOCK_STREAM, IPPROTO_TCP);
        if (!::connect(sock_fd, (sockaddr*)addr_bytes, (type==Address::IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)))) {
            setNonBlocking(sock_fd);
            LOG(Log::LOGLEVEL_NOTICE, "Outbound connection connected -> %d\r\n", sock_fd);
            return sock_fd;
        }
        
        LOG(Log::LOGLEVEL_NOTICE, "Failed to connect, errno %d", errno);
        
        return 0;
    }

    int Socket::connect(const char* addr, unsigned short port) {
        Address address(Address::ADDR, addr);
        return connect(address, port);
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
        release_lock = new Mutex();
        
        sockets = new LinkedList<Socket*>();
    }

    void Socket::releaseSocketSubsystem() {
        delete descriptors_lock;
        delete descriptors;
        delete release_lock;
        
        delete sockets;
        
#ifdef _WIN32
        WSACleanup();
#endif
    }
    
    LinkedList<Socket*> Socket::getOpenSockets() {
        return *sockets;
    }
    
    void Socket::closeAllSockets() {
        release_lock->lock();
        
        LinkedListState<Socket*> s(sockets);
        int len = s.length();
        
        for (int i=0; i<len; i++) {
            Socket *sock = s.next();
            sock->close();
            sock->release();
        }
        
        release_lock->release();
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
    
    void Socket::setReceiveBufferSize(size_t size) {
        receiver.setBufferSize(size);
    }
    
}
