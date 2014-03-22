//
//  SerializableString.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 22/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "SerializableString.h"

namespace nrcore {
    
    void SerializableString::beforeSerialization() {
        clearSerializationDeclarations();
        declareByteArray(_length, strbuf);
    }

};
