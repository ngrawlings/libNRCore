//
//  MysqlConnectionPool.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 20/11/2012.
//  Copyright (c) 2013. All rights reserved.
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

#ifndef __PeerConnector__MysqlConnectionPool__
#define __PeerConnector__MysqlConnectionPool__

#include <libnrcore/Threading/Thread.h>

#include "LinkedList.h"

#include "Threading/Mutex.h"
#include "Threading/ThreadWaitCondition.h"

#define HANDLE          void*

template <class T>
class InstancePool {
public:
    typedef enum {
        WAIT,
        BUSY
    } STATUS;

    typedef struct {
        T inst;
        HANDLE node;
        STATUS status;
    } INSTANCE;

public:
    InstancePool(bool auto_free) : wait_count(0) {
        this->auto_free = auto_free;
    }

    virtual ~InstancePool() {
        wait_instances.clear();
        while (instances.length()) {
            if (auto_free)
                delete instances.get(instances.firstNode()).get().inst;
            instances.remove(instances.firstNode());
        }
    }

    void add(const T &obj) {
        mutex->lock();
        HANDLE hnd = instances.add(Ref<INSTANCE>(createIntanceEntry(obj)));
        instances.get(hnd).get().node = hnd;
        wait_instances.add(instances.get(hnd));
        mutex->release();
    }
    
    void remove(const T &obj) {
        LinkedListState< Ref< INSTANCE > > instance_list(&instances);
        int cnt = instance_list.length();
        while(cnt--) {
            if (instance_list.get().get().inst == obj) {
                instance_list.remove();
                break;
            } else
                instance_list.next();
        }
    }
    
    int length() {
        return instances.length();
    }

    HANDLE acquire(bool block) {
        INSTANCE *inst = 0;
        while (true) {
            if (!wait_instances.length()) {
                if (block) {
                    mutex->lock();

                    if (!wait_instances.length()) {
                        mutex->release();
                        wait_count++;
                        wait_mutex->wait(wait_trigger);
                        wait_mutex->release();
                        wait_count--;
                    }
                } else {
                    mutex->release();
                    break;
                }
            }

            mutex->lock();

            if (wait_instances.length()) {
                inst = &wait_instances.get(wait_instances.firstNode()).get();
                wait_instances.remove( wait_instances.firstNode() );
                mutex->release();
                break;
            }

            mutex->release();
        }

        if(inst)
            inst->status = BUSY;

        if (mutex->isLockedByMe())
            mutex->release();
        
        return inst->node;
    }

    const T &getInstance(HANDLE handle) {
        return instances.get(handle).get().inst;
    }

    void release(HANDLE handle) {
        mutex->lock();
        Ref<INSTANCE> inst = instances.get(handle);
        if (inst.get().status == BUSY) {
            inst.get().status = WAIT;
            wait_instances.add(inst);
            wait_trigger->trigger();
        } else {
            mutex->release();
            throw "Invalid State";
        }
        mutex->release();
    }

    void autoFree(bool auto_free) {
        this->auto_free = auto_free;
    }
    
    int getSize() {
        return instances.length();
    }
    
    T get(int index) {
        if (index > instances.length())
            throw "Invalid Index";
        
        LINKEDLIST_NODE_HANDLE node = instances.firstNode();
        while (index--)
            node = instances.nextNode(node);
        
        return instances.get(node).get().inst;
    }

private:

    LinkedList< Ref< INSTANCE > > instances;
    LinkedList< Ref< INSTANCE > > wait_instances;

    static ThreadWaitCondition *wait_trigger;
    static Mutex *wait_mutex;
    static Mutex *mutex;

    bool auto_free;
    int wait_count;

    INSTANCE *createIntanceEntry(T obj) {
	INSTANCE *inst = new INSTANCE;
	inst->inst = obj;
	inst->node = 0;
	inst->status = InstancePool<T>::WAIT;
	return inst;
    }
};


template <class T>
ThreadWaitCondition  *InstancePool<T>::wait_trigger = new ThreadWaitCondition();

template <class T>
Mutex *InstancePool<T>::wait_mutex = new Mutex("wait_thread");

template <class T>
Mutex *InstancePool<T>::mutex = new Mutex("InstancePool Mutex");


#endif /* defined(__PeerConnector__MysqlConnectionPool__) */
