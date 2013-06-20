//
//  Thread.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 19/11/2012.
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

#include "Thread.h"
#include <libnrcore/debug/Log.h>

#if __APPLE__
#include <sys/time.h>
#include <unistd.h>
#endif

namespace nrcore {

    LinkedList<Thread*> *Thread::threads = 0;
    LinkedList<Thread*> *Thread::wait_threads = 0;
    Mutex *Thread::threads_mutex = 0;
    Mutex *Thread::wait_threads_mutex = 0;
    ThreadWaitCondition  *Thread::wait_any_thread_trigger = 0;
    Mutex *Thread::wait_any_thread_mutex = 0;

    bool Thread::pool = true;
    int Thread::max_threads = 64;

    pthread_key_t Thread::tlsKey = 0;

    Thread::Thread() {
        _run = true;

        if (!tlsKey)
            pthread_key_create(&tlsKey, 0);

        threads_mutex->lock();
        
        linkedlist_node_hanble = threads->add(this);
        pthread_create( &thread, NULL, threadEntry, (void*)this);
        
        threads_mutex->release();
    }

    Thread::~Thread() {
        _run = false;
        
        if (linkedlist_node_hanble) {
            threads_mutex->lock();
            
            threads->remove(linkedlist_node_hanble);
            
            if (thread) {
                if (status == THREAD_WAITING)
                    wake();
            }
            
            linkedlist_node_hanble = 0;
            
            threads_mutex->release();
            
            if (thread && !pthread_kill(thread, 0))
                pthread_join(thread, 0);
        }
    }

    void Thread::init(int thread_count) {
        threads = new LinkedList<Thread*>();
        wait_threads = new LinkedList<Thread*>();
        threads_mutex = new Mutex("threads_mutex", false);
        wait_threads_mutex = new Mutex("wait_threads_mutex", false);
        wait_any_thread_trigger = new ThreadWaitCondition();
        wait_any_thread_mutex = new Mutex("wait_any_thread_mutex", false);
        
        for (int i=0; i<thread_count; i++)
            Thread::addThread();
    }

    void *Thread::threadEntry( void *inst ) {
        pthread_setspecific(tlsKey, inst);
        
        ((Thread*)inst)->thread_loop();
        ((Thread*)inst)->thread = 0;
        
        if (((Thread*)inst)->linkedlist_node_hanble)
            delete ((Thread*)inst);
        return 0;
    }

    Thread *Thread::addThread() {
        return new Thread();
    }

    void Thread::thread_loop() {
        Task *task;
        while(_run) {
            try {
                
                while ((task = dynamic_cast<Task*>(Task::getNextTask())))
                    task->run();
                
            } catch (...) {
                logger.log("Invalid Task in queue");
            }
            
            if (!pool || threads->length() > max_threads)
                break;
            
            wait();
        }
    }

    void Thread::wait() {
        wait_threads_mutex->lock();
        
        if (locked_mutex_list.length()) {
            Mutex* mutex;
            
            LinkedList<Mutex*> mcpy;
            mcpy.copy(&locked_mutex_list);
            
            LinkedListState<Mutex*> mutex_list(&mcpy);
            while (mutex_list.length()) {
                mutex = mutex_list.get();
                
                if (mutex->isManaged() && mutex->isLockedByMe()) {
                    logger.log("WARNING: Mutex not released (Auto Release) %s (%s)", mutex->tag(), mutex->lockTag());
                    mutex->release();
                }
                
                mutex_list.remove();
            }
            
            locked_mutex_list.clear();
        }
        
        status = THREAD_WAITING;
        wait_for_thread_trigger.trigger();
        
        wait_threads->add(this);
        
        finished();
        wait_threads_mutex->release();
        
        mutex.wait(&trigger);
        mutex.release();
        status = THREAD_ACTIVE;
    }

    void Thread::waitUntilFinished() {
        wait_threads_mutex->lock();
        
        if (status != THREAD_WAITING) {
            
            wait_threads_mutex->release();
            
            wait_for_thread_finish.wait(&wait_for_thread_trigger);
            wait_for_thread_finish.release();
        } else
            wait_threads_mutex->release();
    }

    void Thread::signal(int sig) {
        pthread_kill(thread, sig);
        switch (sig) {
            case SIGHUP:
            case SIGTERM:
            case SIGQUIT:
                thread = 0;
                break;
            default:
                break;
        }
    }

    void Thread::wake() {
        trigger.trigger();
    }

    void Thread::finished() {
        wait_any_thread_trigger->trigger();
    }

    Thread *Thread::runTask(Task *task) {
        Thread *thrd = 0;
        
        if (task)
            Task::queueTask(task);
        
        wait_threads_mutex->lock();
            
        if (wait_threads->length()) {
            try {
                thrd = dynamic_cast<Thread*>(wait_threads->get(wait_threads->firstNode()));
                if (thrd) {
                    wait_threads->remove( wait_threads->firstNode() );
                        //logger.log("Waking Thread %p, Wait Thread Count %d", thrd, wait_threads.length());
                        
                    wait_threads_mutex->release();
                        
                    thrd->wake();

                } 
            } catch (...) {
                    logger.log("An Exception Occurred within a task");
            }
        }
            
        if (wait_threads_mutex->isLockedByMe())
            wait_threads_mutex->release();

        return thrd;
    }

    void Thread::staticCleanUp() {
        wait_threads_mutex->lock();
        
        wait_threads->clear();
        pool = false;
        
        LinkedList<Thread*> thread_list_copy;
        thread_list_copy.copy(threads);
        
        LinkedListState<Thread*> thread_list(&thread_list_copy);
        
        while(thread_list.length()) {
            delete thread_list.get();
            thread_list.remove();
        }
        
        if (!threads->length()) {
            logger.log("Releasing up threading system");
            
            delete threads;
            delete wait_threads;
            delete wait_threads_mutex;
            delete wait_any_thread_trigger;
            delete wait_any_thread_mutex;
            
            threads = 0;
            wait_threads = 0;
            wait_threads_mutex = 0;
            wait_any_thread_trigger = 0;
            wait_any_thread_mutex = 0;
        }
    }

    void Thread::waitForAnyAvailableThread() {
        wait_threads_mutex->lock();
        if (!wait_threads->length()) {
            wait_threads_mutex->release();
            wait_any_thread_mutex->wait(wait_any_thread_trigger);
            wait_any_thread_mutex->release();
        }
    }

    Thread* Thread::getThreadInstance() {
        return (Thread*)pthread_getspecific(tlsKey);
    }

    void Thread::mutexLocked(Mutex* mutex) {
        Thread* thrd = getThreadInstance();
        if (thrd)
            thrd->locked_mutex_list.add(mutex);
    }

    void Thread::mutexReleased(Mutex* mutex) {
        Thread* thrd = getThreadInstance();
        if (thrd)
            thrd->locked_mutex_list.remove(mutex);
    }

    void Thread::getThreadPoolState(int *total, int *active) {
        *total = threads->length();
        *active = (*total) - wait_threads->length();
    }

    void Thread::stopAllThreads() {
        Thread *thread;
        for (int i=0; i<threads->length(); i++) {
            thread = threads->get(i);
            thread->_run = false;
            thread->wake();
        }
    }
    
}