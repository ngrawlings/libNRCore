//
//  Semaphore.cpp
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

#include "Semaphore.h"

namespace nrcore {

    Semaphore::Semaphore(int max) : count(0), count_mutex("count_mutex") {
        this->max = max;
        pthread_mutex_init(&wait_mutex, 0);
        pthread_cond_init(&condition, 0);
    }

    Semaphore::~Semaphore() {
        pthread_mutex_destroy(&wait_mutex);
        pthread_cond_destroy(&condition);
    }

    void Semaphore::wait() {
        if (count)
            pthread_cond_wait(&condition, &wait_mutex);
    }

    void Semaphore::increment() {
        count_mutex.lock();
        if (max && max==count) {
            count_mutex.release();
            throw "Max locks";
        }
        count++;
        count_mutex.release();
    }

    void Semaphore::decrement() {
        count_mutex.lock();
        count--;
        
        if (!count)
            pthread_cond_broadcast(&condition);
        count_mutex.release();
    }

}