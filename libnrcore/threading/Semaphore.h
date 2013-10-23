//
//  Semaphore.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 06/01/2013.
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

#ifndef __PeerConnector__Semaphore__
#define __PeerConnector__Semaphore__

#include "Thread.h"

namespace nrcore {

    class Semaphore {
    public:
        friend class SemaphoreLock;
        
        Semaphore(int max=0);
        ~Semaphore();
        
        void wait();
        
    protected:
        void increment();
        void decrement();
        
    private:
        int count;
        int max;

        thread_mutex_t wait_mutex;
        thread_cond_t  condition;
        
        Mutex count_mutex;
    };
    
};

#endif /* defined(__PeerConnector__Semaphore__) */
