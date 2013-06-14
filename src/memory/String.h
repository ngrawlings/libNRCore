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

#include <unistd.h>
#include <string.h>

#include "Array.h"

#define STRREF StringRef<String>
#define T(x) String(x)

namespace nrcore {
    
    template <class T>
    class StringRef : public Ref<T> {
    public:
        explicit StringRef<T>(T* ptr, bool array=false) : Ref<T>(ptr, array) {}
        StringRef<T>(const Ref<T>& ref) : Ref<T>(ref) {}
        StringRef<T>() : Ref<T>() {}

        operator char*() {
            return this->get();
        }

        StringRef<T> operator +(StringRef<T> str) {
            return this->getPtr()->operator +(str);
        }

        StringRef<T> operator +(T str) {
            return this->getPtr()->operator +(str);
        }
        
        //StringRef<T> operator +(unsigned int num) {
        //    return this->getPtr()->operator +(num);
        //}
        
        StringRef<T> operator +(long num) {
            return this->getPtr()->operator +(num);
        }
        
        //StringRef<T> operator +(unsigned long num) {
        //    return this->getPtr()->operator +(num);
        //}
        
        StringRef<T> operator +(long long num) {
            return this->getPtr()->operator +(num);
        }
        
        StringRef<T> operator +(unsigned long long num) {
            return this->getPtr()->operator +(num);
        }
        
    };

    class String {
    public:
        String() : strbuf(0), size(0), _length(0) {}
        String(const char *str);
        String(String &str);
        String(STRREF str);
        String(const char c);
        
        String(int num);
        String(unsigned int num);
        
        String(long num);
        String(unsigned long num);
        
        String(long long num);
        String(unsigned long long num);
        
        virtual ~String();
        
        ssize_t length() {
        	return _length;
        }

        operator char*() {
            return strbuf;
        }
        
        String &operator =(const char *str) {
            _length = 0;
            size_t len = strlen(str);
            allocateBlock(len);
            _length = len;
            memcpy(strbuf, str, len+1);
            return *this;
        }
        
        String &operator =(String &str) {
        	_length = 0;
        	size_t len = str._length;
        	allocateBlock(len);
        	_length = len;
        	memcpy(strbuf, str.strbuf, len+1);
        	return *this;
        }

        String &operator =(STRREF str) {
            _length = 0;
            size_t len = str.get()._length;
            allocateBlock(len);
            _length = len;
            memcpy(strbuf, str.get().strbuf, len+1);
            return *this;
        }
        
        String &operator +=(const char* str) {
            append(String(str));
            return *this;
        }
        
        String &operator +=(int num) {
            return this->operator +=(num2str(num).get());
        }
        
        String &operator +=(unsigned int num) {
            return this->operator +=(unum2str(num).get());
        }
        
        String &operator +=(long num) {
            return this->operator +=(num2str(num).get());
        }
        
        String &operator +=(unsigned long num) {
            return this->operator +=(unum2str(num).get());
        }
        
        String &operator +=(long long num) {
            return this->operator +=(num2str(num).get());
        }
        
        String &operator +=(unsigned long long num) {
            return this->operator +=(unum2str(num).get());
        }
        
        String &operator +=(STRREF str) {
            append(str.get());
            return *this;
        }
        
        String &operator +=(String &str) {
            append(str);
            return *this;
        }
        
        String &operator +=(String *str) {
            append(*str);
            return *this;
        }
        
        String &operator +=(const char c) {
            if (_length+1 >= size)
                allocateBlock(_length+1);
                
            strbuf[_length+1] = 0;
            strbuf[_length] = c;
            _length++;
            
            return *this;
        }
        
        STRREF operator +(const char *str) {
            String *ret = new String(*this);
            ret->operator +=(String(str));
            return STRREF(ret);
        }
        
        STRREF operator +(const char c) {
            String *ret = new String(*this);
            ret->operator +=(c);
            return STRREF(ret);
        }
        
        STRREF operator +(STRREF str) {
            return this->operator+(str.get());
        }
        
        STRREF operator +(String *str) {
            return this->operator+(*str);
        }
        
        STRREF operator +(String &str) {
            String *ret = new String(*this);
            ret->operator += (str);
            return STRREF(ret);
        }
        
        STRREF operator +(long long num) {
            return this->operator +(num2str(num));
        }
        
        STRREF operator +(unsigned long long num) {
            return this->operator +(unum2str(num));
        }
        
        void append(const String &str);
        Ref< Array<String*> > split(const char* delimiter, bool ignore_zero_len=true, int limit=0);
        
        
    protected:
        char *strbuf;
        size_t size, _length;
        
    private:
        void allocateBlock(size_t min_sz);
        STRREF num2str(long long num);
        STRREF unum2str(unsigned long long num);
    };
    
};

#endif /* defined(__PeerConnector__NString__) */
