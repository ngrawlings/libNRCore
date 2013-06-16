//
//  Task.h
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

#ifndef __PeerConnector__Task__
#define __PeerConnector__Task__

#include <pthread.h>
#include <libnrcore/memory/LinkedList.h>
#include "Mutex.h"

namespace nrcore {

    class Task {
    public:
        Task(const char* tag);
        Task(pthread_t thread_id);

        virtual ~Task();

        static void queueTask(Task *task);
        static void removeTasks(Task *task);
        static Task* getNextTask();
        
        static int getQueuedTaskCount();
        
        static void staticInit();
        static void staticCleanup();
        
        const char* getTag();

    protected:
        friend class Thread;
        virtual void run() = 0;
        
        unsigned long getThreadId();

        pthread_t exec_thread_id; // if this is not zero only a thread with a matching id can run this task;
        void *acquired_thread; // thread instance pointer that is currently executing this task, 0 for not being executed

    private:
        static Mutex *task_queue_mutex;
        static LinkedList<Task*> *task_queue;
        static LinkedList<Task*> *task_queue_thread_specific;
        const char* tag;
    };

}

#endif /* defined(__PeerConnector__Task__) */
