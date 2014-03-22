//
//  SerializableString.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 22/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__SerializableString__
#define __libNRCore__SerializableString__

#include <libnrcore/memory/String.h>
#include <libnrcore/serializers/Serializable.h>

namespace nrcore {
  
    class SerializableString : public String, public Serializable {
    protected:
        void beforeSerialization();
    };
    
};

#endif /* defined(__libNRCore__SerializableString__) */
