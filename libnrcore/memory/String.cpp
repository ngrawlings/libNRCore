//
//  String.cpp
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

#include "String.h"

#include <string.h>
#include "LinkedList.h"
#include "Array.h"

namespace nrcore {

    String::String(const char *str) : strbuf(0), _length(0) {
        size_t len = strlen(str);
        allocateBlock(len);
        _length = len;
        memcpy(strbuf, str, _length+1);
    }

    String::String(const String &str) : strbuf(0), _length(0) {
        allocateBlock(str._length);
        _length = str._length;
        memcpy(strbuf, str.strbuf, _length+1);
    }
    
    String::String(const char c) : strbuf(0), _length(0) {
        allocateBlock(1);
        strbuf[0] = c;
        strbuf[1] = 0;
    }
    
    String::String(int num) : strbuf(0), _length(0) {
        this->operator =(num2str(num));
    }
    
    String::String(unsigned int num) : strbuf(0), _length(0) {
        this->operator =(unum2str(num));
    }
    
    String::String(long num) : strbuf(0), _length(0) {
        this->operator =(num2str(num));
    }
    
    String::String(unsigned long num) : strbuf(0), _length(0) {
        this->operator =(unum2str(num));
    }

    String::String(long long num) : strbuf(0), _length(0) {
        this->operator =(num2str(num));
    }
    
    String::String(unsigned long long num) : strbuf(0), _length(0) {
        this->operator =(unum2str(num));
    }

    String::~String() {
        if (strbuf)
            delete [] strbuf;
    }

    void String::allocateBlock(size_t min_sz) {
        size_t mask = -16;
        size = ((min_sz+1)&mask)+16;
        char *block = new char[size];
        
        if (_length && strbuf != 0)
            memcpy(block, strbuf, _length);
        
        if (strbuf)
            delete [] strbuf;
        
        strbuf = block;
    }

    String String::num2str(long long num) {
        char tmp[32], d;
        int i=0, len;
        bool neg = num < 0;

        do {
            tmp[i] = (num%10)+48;
            i++;
        } while (num/=10);
        
        len = i;
        tmp[i] = 0;
        
        for (i=0; i<len/2; i++) {
            d = tmp[i];
            tmp[i] = tmp[len-i-1];
            tmp[len-i-1] = d;
        }

        return String(neg ? "-" : "") + String(tmp);
    }
    
    String String::unum2str(unsigned long long num) {
        char tmp[32], d;
        int i=0, len;
        
        do {
            tmp[i] = (num%10)+48;
            i++;
        } while (num/=10);
        
        len = i;
        tmp[i] = 0;
        
        for (i=0; i<len/2; i++) {
            d = tmp[i];
            tmp[i] = tmp[len-i-1];
            tmp[len-i-1] = d;
        }
        
        return String(tmp);
    }
    
    void String::append(const String &str) {
        if (size <= _length+str._length+1)
            allocateBlock(_length+str._length);
        memcpy(&strbuf[_length], str.strbuf, str._length+1);
        _length += str._length;
    }
    
    Ref< Array<String*> > String::split(const char* delimiter, bool ignore_zero_len, int limit) {
        LinkedList<char*> parts;
        char *buf = new char[_length+1];
        memcpy(buf, strbuf, _length+1);
        size_t dlen = strlen(delimiter);
        char *del;
        
        parts.add(buf);
        del = buf;
        while (parts.length() < limit || !limit) {
            del = strstr(del, delimiter);
            
            if (!del)
                break;
            
            del[0] = 0;
            del += dlen;
            
            parts.add(del);
        }
        
        Array<String*> *sa = new Array<String*>(0);
        sa->autoRelease(true);
        
        LinkedListState<char*> pl(&parts);
        
        while (pl.length()) {
            if (ignore_zero_len && !strlen(pl.get())) {
                pl.remove();
                continue;
            }
            
            String *str = new String(pl.get());
            sa->push(str);
            pl.remove();
        }
        
        delete [] buf;
        
        return Ref< Array<String*> >(sa);
    }
    
    int String::indexOf(String search) {
        bool found;
        int slen = search.length();
        
        for (int i=0; i<_length-slen; i++) {
            found = true;
            for (int x=0; x<slen; x++) {
                found = strbuf[i+x] == search.strbuf[x];
                if (!found)
                    break;
            }
            if (found)
                return i;
        }
        return -1;
    }
    
    String String::substr(int offset, int length) {
        length = length ? length : (int)_length-offset;
        
        char *buf = new char[length];
        
        memcpy(buf, &strbuf[offset], length);
        buf[length] = 0;
        
        String ret(buf);
        delete buf;
        
        return ret;
    }
    
    String &String::insert(int index, String ins) {
        int ins_len = ins.length();
        
        if (_length+ins_len >= size)
            allocateBlock(_length+ins_len);
        
        for (int i=_length; i>=index; i--)
            strbuf[i+ins_len] = strbuf[i];
        
        memcpy(&strbuf[index], ins.operator char *(), ins_len);
        
        _length += ins_len;
        
        return *this;
    }
    
    String &String::replace(String search, String replace, int maxcnt) {
        int cnt=0, index, size_dif, slen, rlen;
        
        slen = search.length();
        rlen = replace.length();
        size_dif = rlen - slen;
        
        while ((index = indexOf(search)) != -1) {
            if (maxcnt > 0 && cnt == maxcnt)
                break;
            
            if (size_dif > 0) {
                if (_length+size_dif >= size)
                    allocateBlock(_length+size_dif);
        
                for (int i=_length; i>=index+slen; i--)
                    strbuf[i+size_dif] = strbuf[i];
            } else if (size_dif < 0) {
                for (int i=index+rlen; i<_length; i++)
                    strbuf[i] = strbuf[i-size_dif];
            }
            
            memcpy(&strbuf[index], replace.operator char *(), rlen);
            
            _length += size_dif;
            
            cnt++;
        }
        
        return *this;
    }
    
    String &String::arg(String arg) {
        replace("%", arg, 1);
        return *this;
    }
    
};
