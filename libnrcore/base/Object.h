//
//  Object.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 03/08/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Object__
#define __libNRCore__Object__

#define OBJECT_EVENT_INSTANCIATE    0
#define OBJECT_EVENT_DESTROYED      1

namespace nrcore {
    
    class Object {
    public:
        Object();
        virtual ~Object();
        
        virtual void signal(Object *obj, int event_op) {};
      
    };
    
}

#endif /* defined(__libNRCore__Object__) */
