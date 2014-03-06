//
//  RegularExpression.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 04/03/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__RegularExpression__
#define __libNRCore__RegularExpression__

#include <regex.h>

#include "String.h"

namespace nrcore {

    class RegularExpression {
    public:
        RegularExpression(String expr);
        RegularExpression(const RegularExpression &regex);
        virtual ~RegularExpression();
        
        String &getExpression();
        
        bool match(String str);

        
    protected:
        String expr;
        regex_t regex;
    };

};

#endif /* defined(__libNRCore__RegularExpression__) */
