//
//  String.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 16/11/2012.
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

#ifndef __PeerConnector__String__
#define __PeerConnector__String__

#include "Ref.h"

#include <stdio.h>
#include <string.h>

#include "Array.h"

namespace nrcore {

    class String {
    public:
        String() : strbuf(0), size(0), _length(0) {}
        String(const char *str);
        String(const char *str, size_t len);
        String(const String &str);
        String(const char c);
        
        String(int num);
        String(unsigned int num);
        
        String(long num);
        String(unsigned long num);
        
        String(long long num);
        String(unsigned long long num);
        
        String(double num);
        String(long double num);
        
        virtual ~String();
        
        ssize_t length() {
        	return _length;
        }

        operator char*() {
            return strbuf;
        }
        
        String &operator =(String str) {
        	_length = 0;
        	size_t len = str._length;
        	allocateBlock(len);
        	_length = len;
            if (len)
                memcpy(strbuf, str.strbuf, len+1);
        	return *this;
        }
        
        String &operator +=(String str) {
            if (str.length())
                append(str);
            return *this;
        }
        
        String operator +(String str) const {
            String _s(*this);
            _s += str;
            return _s;
        }
        
        bool operator== (const String &str) const
        {
            return _length == str._length && !memcmp(strbuf, str.strbuf, _length);
        }
        
        bool operator!= (String &str) const
        {
            return !(*this == str);
        }
        
        void append(const String &str);
   
        int indexOf(String search, int start=0);
        int occuranceCount(String search);
        
        String substr(int offset, int length=0);
        String &insert(int index, String ins);
        String &replace(String search, String replace, int offset=0, int maxcnt=0);
        
        bool equals(String str, bool case_insensitive);
        
        bool startsWith(String str);
        bool endsWith(String str);
        
        String &arg(String arg, const char* replace_marker=0);
        
        String &escape();
        
        String extract(String start, String end, int *offset=0);
        
        String trim(const char *tchrs=0);
        
        static String urlDecode(String str);
        
    protected:
        char *strbuf;
        size_t size, _length;
        
        void allocateBlock(size_t min_sz);
    };
    
};

#endif /* defined(__PeerConnector__NString__) */
