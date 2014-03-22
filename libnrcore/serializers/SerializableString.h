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
    public:
        SerializableString() {}
        SerializableString(const char *str);
        SerializableString(const String &str);
        SerializableString(const char c);
        
        SerializableString(int num);
        SerializableString(unsigned int num);
        
        SerializableString(long num);
        SerializableString(unsigned long num);
        
        SerializableString(long long num);
        SerializableString(unsigned long long num);
        
        SerializableString(double num);
        SerializableString(long double num);
        
        virtual ~SerializableString();
        
    protected:
        void beforeSerialization();
        void serializedObjectLoaded(int index, SERIAL_OBJECT *so);
    };
    
};

#endif /* defined(__libNRCore__SerializableString__) */
