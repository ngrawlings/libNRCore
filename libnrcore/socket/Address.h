//
//  Address.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 25/03/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Address__
#define __libNRCore__Address__

namespace nrcore {

    class Address {
    public:
        enum ADDRESS_TYPE {
            IPV4,   // binary format
            IPV6,   // binary format
            ADDR,   // Can be a text representation of any type of address including domains, it will be automatically resolved
            DOMAIN, // This will not be resolved but just make a direct copy of the name
            MAC     // binary format
        };
        
        Address(ADDRESS_TYPE type, const char* bytes);
        virtual ~Address();
        
        ADDRESS_TYPE getType();
        int getAddrSize();
        const char* getAddr();
        
        static Address::ADDRESS_TYPE getType(const char* addr);
        
    protected:
        ADDRESS_TYPE type;
        char *addr;

    };
    
};

#endif /* defined(__libNRCore__Address__) */
