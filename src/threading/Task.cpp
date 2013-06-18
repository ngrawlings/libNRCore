//
//  Task.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 12/02/2013.
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

#include "Task.h"
#include <libnrcore/debug/Log.h>

#include <assert.h>
#include <libnrcore/threading/Thread.h>

namespace nrcore {

    Mutex *Task::task_queue_mutex = 0;
    LinkedList<Task*> *Task::task_queue = 0;
    LinkedList<Task*> *Task::task_queue_thread_specific = 0;

    Task::Task(const char *tag) {
        exec_thread_id = 0;
        acquired_thread = 0;
        this->tag = tag;
    }

    Task::Task(pthread_t thread_id) {
        this->exec_thread_id = thread_id;
        acquired_thread = 0;
    }

    Task::~Task() {
        removeTasks(this);
        
        if ( acquired_thread && acquired_thread != Thread::getThreadInstance() )
            reinterpret_cast<Thread*>(acquired_thread)->waitUntilFinished();
    }

    void Task::queueTask(Task *task) {
        //logger.log("Adding Task %s", task->getTag());
        
        task_queue_mutex->lock();
        
        if (!task->exec_thread_id)
            task_queue->add(task);
        else
            task_queue_thread_specific->add(task);
        
        task_queue_mutex->release();
    }

    void Task::removeTasks(Task *task) {
        task_queue_mutex->lock();
        
        LINKEDLIST_NODE_HANDLE node;
        
        if (task_queue->length()) {
            node = task_queue->firstNode();
            
            if (node) {
                do {
                    
                    if (task_queue->get(node) == task)
                        task_queue->remove(node);
                    
                    node = task_queue->nextNode(node);
                    
                } while (task_queue->length() && node != task_queue->firstNode());
            }
        }
        
        if (task_queue_thread_specific->length()) {
            node = task_queue_thread_specific->firstNode();
            if (node) {
                do {
                    
                    if (task_queue_thread_specific->get(node) == task)
                        task_queue_thread_specific->remove(node);
                    
                    node = task_queue_thread_specific->nextNode(node);
                    
                } while (task_queue_thread_specific->length() && node != task_queue_thread_specific->firstNode());
            }
        }
        
        task_queue_mutex->release();
    }

    Task* Task::getNextTask() {
        Task *ret=0;
        
        try {
            if (task_queue_mutex->lock()) {
                int len = task_queue_thread_specific->length();
                LinkedListState<Task*> specific_task_list(task_queue_thread_specific);
                
                if (len) {
                    
                    ret = specific_task_list.get();
                    for (int i=0; i<len; i++) {
                        if (ret && ret->exec_thread_id == pthread_self())
                            break;
                        ret = specific_task_list.next();
                    }
                }
                
                if (!ret || (ret && ret->exec_thread_id != pthread_self())) {

                    if (task_queue->length()) {

                        LINKEDLIST_NODE_HANDLE node = task_queue->firstNode();
                        ret = task_queue->get(node);
                        task_queue->remove(node);

                    }
                    
                } else {
                    specific_task_list.remove();
                }
                
            }
        } catch (...) {
            logger.log("there was an exception in getNextTask");
        }
        
        if (ret)
            ret->acquired_thread = Thread::getThreadInstance();
        
        if (task_queue_mutex->isLockedByMe())
            task_queue_mutex->release();

        return ret;
    }

    unsigned long Task::getThreadId() {
        return (unsigned long)pthread_self();
    }

    void Task::staticInit() {
        task_queue_mutex = new Mutex("task_queue_mutex");
        task_queue = new LinkedList<Task*>();
        task_queue_thread_specific = new LinkedList<Task*>();
    }

    void Task::staticCleanup() {
        delete task_queue_mutex;
        delete task_queue;
        delete task_queue_thread_specific;
    }

    const char* Task::getTag() {
        return tag;
    }

    int Task::getQueuedTaskCount() {
        return task_queue->length() + task_queue_thread_specific->length();
    }
    
}
