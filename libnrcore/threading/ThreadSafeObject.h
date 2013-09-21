//
//  ThreadSafeObject.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 9/21/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__ThreadSafeObject__
#define __libNRCore__ThreadSafeObject__

#include "Mutex.h"

namespace nrcore {
    
    template <class T>
    class ThreadSafeObject {
    public:
        ThreadSafeObject(T obj) {
            this->obj = obj;
        }
        virtual ~ThreadSafeObject() {}
        
        T& acquire() {
            mutex.lock();
            return obj;
        }
        
        void release() {
            mutex.release();
        }
        
    private:
        T obj;
        Mutex mutex;
    };

};

#endif /* defined(__libNRCore__ThreadSafeObject__) */
