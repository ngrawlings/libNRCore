//
//  ObjectEvent.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 03/08/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__ObjectEvent__
#define __libNRCore__ObjectEvent__

namespace nrcore {

    class ObjectEvent {
    public:
        ObjectEvent() {}
        virtual ~ObjectEvent() {}
        virtual void event(void* inst, int event_op) = 0;
        
    };

}
               
#endif /* defined(__libNRCore__ObjectEvent__) */
