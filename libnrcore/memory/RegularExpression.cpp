//
//  RegularExpression.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 04/03/14.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#include "RegularExpression.h"

namespace nrcore {

    RegularExpression::RegularExpression(String expr) {
        this->expr = expr;
        if (regcomp(&regex, this->expr, REG_EXTENDED))
            throw "Failed to Compile";
    }
    
    RegularExpression::RegularExpression(const RegularExpression &regex) {
        this->expr = regex.expr;
        if (regcomp(&this->regex, this->expr, REG_EXTENDED))
            throw "Failed to Compile";
    }

    RegularExpression::~RegularExpression() {
        regfree(&regex);
    }
    
    String &RegularExpression::getExpression() {
        return expr;
    }

    bool RegularExpression::RegularExpression::match(String str) {
        return !regexec(&regex, str, 0, NULL, 0);
    }
    
}
