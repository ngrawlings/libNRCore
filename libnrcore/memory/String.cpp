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

#include <ctype.h>

namespace nrcore {

    String::String(const char *str) : strbuf(0), size(0), _length(0) {
        size_t len = str ? strlen(str) : 0;
        if (len)
            allocateBlock(len);
        
        _length = len;
        
        if (_length)
            memcpy(strbuf, str, _length+1);
    }
    
    String::String(const char *str, size_t len) : strbuf(0), size(0), _length(0) {
        if (len)
            allocateBlock(len);
        
        _length = len;
        
        if (_length) {
            memcpy(strbuf, str, _length);
            strbuf[_length] = 0;
        }
    }

    String::String(const String &str) : strbuf(0), size(0), _length(0) {
        if (str._length) {
            allocateBlock(str._length);
            _length = str._length;
            memcpy(strbuf, str.strbuf, _length+1);
        }
    }
    
    String::String(const char c) : strbuf(0), size(0), _length(1) {
        allocateBlock(1);
        strbuf[0] = c;
        strbuf[1] = 0;
    }
    
    String::String(int num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%d", num);
        this->operator =(buf);
    }
    
    String::String(unsigned int num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%u", num);
        this->operator =(buf);
    }
    
    String::String(long num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%ld", num);
        this->operator =(buf);
    }
    
    String::String(unsigned long num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%lu", num);
        this->operator =(buf);
    }

    String::String(long long num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%lld", num);
        this->operator =(buf);
    }
    
    String::String(unsigned long long num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%llu", num);
        this->operator =(buf);
    }
    
    String::String(double num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%e", num);
        this->operator =(buf);
    }
    
    String::String(long double num) : strbuf(0), size(0), _length(0) {
        char buf[32];
        sprintf(buf,"%Le", num);
        this->operator =(buf);
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
    
    void String::append(const String &str) {
        if (str._length) {
            if (size <= _length+str._length+1)
                allocateBlock(_length+str._length);
            
            memcpy(&strbuf[_length], str.strbuf, str._length+1);
            _length += str._length;
        }
    }
    
    int String::indexOf(String search, int start) {
        bool found;
        ssize_t slen = search.length();
        
        if (!_length || !slen)
            return -1;
        
        for (unsigned int i=start; i<=_length-slen; i++) {
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
    
    int String::occuranceCount(String search) {
        int offset = 0;
        int cnt = 0;
        
        while ((offset = indexOf(search, offset)) != -1) {
            cnt++;
            offset++;
        }
        
        return cnt;
    }
    
    String String::substr(int offset, int length) {
        length = length ? length : (int)_length-offset;
        
        char *buf = new char[length+1];
        
        memcpy(buf, &strbuf[offset], length);
        buf[length] = 0;
        
        String ret(buf);
        delete [] buf;
        
        return ret;
    }
    
    String &String::insert(int index, String ins) {
        ssize_t ins_len = ins.length();
        
        if (_length+ins_len >= size)
            allocateBlock(_length+ins_len);
        
        for (ssize_t i=_length; i>=index; i--)
            strbuf[i+ins_len] = strbuf[i];
        
        memcpy(&strbuf[index], ins.operator char *(), ins_len);
        
        _length += ins_len;
        
        return *this;
    }
    
    String &String::replace(String search, String replace, int offset, int maxcnt) {
        ssize_t cnt=0, index, size_dif, slen, rlen;
        
        slen = search.length();
        rlen = replace.length();
        size_dif = rlen - slen;
        
        while ((index = indexOf(search, offset)) != -1) {
            if (maxcnt > 0 && cnt == maxcnt)
                break;
            
            if (size_dif > 0) {
                if (_length+size_dif >= size)
                    allocateBlock(_length+size_dif);
        
                for (ssize_t i=_length; i>=index+slen; i--)
                    strbuf[i+size_dif] = strbuf[i];
            } else if (size_dif < 0) {
                for (ssize_t i=index+rlen; i<_length; i++)
                    strbuf[i] = strbuf[i-size_dif];
            }
            
            memcpy(&strbuf[index], replace.operator char *(), rlen);
            
            _length += size_dif;
            strbuf[_length] = 0;
            
            cnt++;
        }
        
        return *this;
    }
    
    bool String::equals(String str, bool case_insensitive) {
        if (_length != str._length)
            return false;
        
        if (case_insensitive) {
            for (int i=0; i<_length; i++) {
                if (strbuf[i] == str.strbuf[i])
                    continue;
                
                if ((strbuf[i] >= 'A' && strbuf[i] <= 'Z') && strbuf[i] == str.strbuf[i]-32)
                    continue;
                
                if ((strbuf[i] >= 'a' && strbuf[i] <= 'z') && strbuf[i] == str.strbuf[i]+32)
                    continue;
                
                return false;
            }
            
            return true;
        }
        
        return !memcmp(strbuf, str.strbuf, _length);
    }
    
    bool String::startsWith(String str) {
        const char *sbuf = str.strbuf;
        ssize_t len = str.length();
        
        for (int i=0; i<len; i++) {
            if (sbuf[i] != strbuf[i])
                return false;
        }
        return true;
    }
    
    bool String::endsWith(String str) {
        const char *sbuf = str.strbuf;
        ssize_t len = str.length();
        
        for (int i=0; i<len; i++) {
            if (sbuf[i] != strbuf[i+(_length-len)])
                return false;
        }
        return true;
    }
    
    String &String::arg(String arg, const char* replace_marker) {
        if (replace_marker)
            replace(replace_marker, arg, 0, 1);
        else
            replace("%", arg, 0, 1);
        
        return *this;
    }
    
    String &String::escape() {
        const char search[] = "\\\'\"\b\n\r\t\%_\x26";
        const char rep[] = "\\\'\"bnrt%_Z";
        int offset;
        
        for (int i=0; i<9; i++) {
            offset = 0;
            while ((offset = indexOf(search[i], offset)) != -1) {
                replace(search[i], String("\\")+String(rep[i]), offset, 1);
                offset += 2;
            }
        }

        return *this;
    }
    
    String String::extract(String start, String end, int *offset) {
        int off = 0;
        
        if (offset)
            off = *offset;
        
        ssize_t start_len = start.length();
        int s = indexOf(start, off);
        if (s != -1) {
            s += start_len;
            ssize_t l = indexOf(end, s);
            if (l == -1)
                l = _length - s;
            else
                l -= s;
            
            return substr(s, (int)l);
        }
        
        return "";
    }
    
    String String::trim(const char *tchrs) {
        ssize_t front_trim, back_trim, x;
        if (!tchrs) tchrs = " \t\r\n\x0";
        size_t len = strlen(tchrs);
        
        for (front_trim=0; front_trim<_length; front_trim++) {
            for (x=0; x<len; x++) {
                if (strbuf[front_trim] == tchrs[x])
                    break;
            }
            if (x == len)
                break;
        }
        
        for (back_trim=_length-1; back_trim>=0; back_trim--) {
            for (x=0; x<len; x++) {
                if (strbuf[back_trim] == tchrs[x])
                    break;
            }
            if (x == len)
                break;
        }
        
        back_trim = _length-(back_trim+1);
        
        if (front_trim + back_trim > _length)
            front_trim = 0;
        
        return substr((int)front_trim, (int)(_length - front_trim - back_trim));
    }

    String String::toUpperCase() {
        String tmp = *this;
        
        for (int i=0; i<_length; i++) {
            if (tmp.strbuf[i] >= 97 && tmp.strbuf[i] <= 122)
                tmp.strbuf[i] -= 32;
        }
        
        return tmp;
    }

    String String::toLowerCase() {
        String tmp = *this;
        
        for (int i=0; i<_length; i++) {
            if (tmp.strbuf[i] >= 65 && tmp.strbuf[i] <= 90)
                tmp.strbuf[i] += 32;
        }
        
        return tmp;
    }
    
    String String::urlDecode(String str) {
        char a, b;
        String ret;
        int index = 0;
  
        while ((index = str.indexOf("%", index)) != -1) {
            if (((a = str[index+1]) && (b = str[index+2])) && (isxdigit(a) && isxdigit(b))) {
                if (a >= 'a')
                    a -= 'a'-'A';
                if (a >= 'A')
                    a -= ('A' - 10);
                else
                    a -= '0';
                if (b >= 'a')
                    b -= 'a'-'A';
                if (b >= 'A')
                    b -= ('A' - 10);
                else
                    b -= '0';
                a = (a<<4)|b;
                
                str = str.replace(str.substr(index, 3), String(a));
            } else
                break;
        }
        
        return str;
    }
    
};
