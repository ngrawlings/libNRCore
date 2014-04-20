//
//  RefArray.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 15/04/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef libNRCore_RefArray_h
#define libNRCore_RefArray_h

#include "Ref.h"

namespace nrcore {

    template <class T>
    class RefArray : public Ref<T> {
        explicit RefArray<T>(T* ptr) : Ref<T>(ptr, true) {
            
        }
    };
    
};

#endif
