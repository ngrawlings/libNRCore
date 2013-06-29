//
//  ByteUtils.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 10/03/2013.
//  Copyright (c) 2013. All rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// For affordable commercial licensing please contact nyhl@ngrawlings.com
//

#include "ByteUtils.h"

#include <string.h>

namespace nrcore {

    Ref<char> ByteUtils::getRandomBytes(int length) {
        char *bytes = new char[length];

        while (length--) {
            bytes[length] = rand() & 0xFF;
        }

        return Ref<char>(bytes, true);
    }

    bool ByteUtils::isNumber(char *str, ssize_t len) {
        if (len==-1)
            len = strlen(str);
        
        for (int i=0; i<len; i++)
            if (str[i] < 48 && str[i] >= 58)
                return false;
        
        return true;
    }

};