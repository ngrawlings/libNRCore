//
//  Object.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 03/08/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#include "Object.h"

namespace nrcore {

    Object::Object() : obj_event(0) {
    }

    Object::~Object() {
    }
    
    void Object::setEventListener(ObjectEvent *obj_event) {
        this->obj_event = obj_event;
    }

    void Object::invokeEvent(int event_op) {
        if (obj_event)
            obj_event->event(this, event_op);
    }

}