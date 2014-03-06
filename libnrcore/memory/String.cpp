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

    String::String(const char *str) : strbuf(0), size(0), _length(0) {
        size_t len = strlen(str);
        allocateBlock(len);
        _length = len;
        memcpy(strbuf, str, _length+1);
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
        if (size <= _length+str._length+1)
            allocateBlock(_length+str._length);
        memcpy(&strbuf[_length], str.strbuf, str._length+1);
        _length += str._length;
    }
    
    int String::indexOf(String search, int start) {
        bool found;
        int slen = search.length();
        
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
        int ins_len = ins.length();
        
        if (_length+ins_len >= size)
            allocateBlock(_length+ins_len);
        
        for (int i=_length; i>=index; i--)
            strbuf[i+ins_len] = strbuf[i];
        
        memcpy(&strbuf[index], ins.operator char *(), ins_len);
        
        _length += ins_len;
        
        return *this;
    }
    
    String &String::replace(String search, String replace, int offset, int maxcnt) {
        int cnt=0, index, size_dif, slen, rlen;
        
        slen = search.length();
        rlen = replace.length();
        size_dif = rlen - slen;
        
        while ((index = indexOf(search, offset)) != -1) {
            if (maxcnt > 0 && cnt == maxcnt)
                break;
            
            if (size_dif > 0) {
                if (_length+size_dif >= size)
                    allocateBlock(_length+size_dif);
        
                for (int i=_length; i>=index+slen; i--)
                    strbuf[i+size_dif] = strbuf[i];
            } else if (size_dif < 0) {
                for (unsigned int i=index+rlen; i<_length; i++)
                    strbuf[i] = strbuf[i-size_dif];
            }
            
            memcpy(&strbuf[index], replace.operator char *(), rlen);
            
            _length += size_dif;
            strbuf[_length] = 0;
            
            cnt++;
        }
        
        return *this;
    }
    
    bool String::startsWith(String str) {
        const char *sbuf = str.strbuf;
        int len = str.length();
        
        for (int i=0; i<len; i++) {
            if (sbuf[i] != strbuf[i])
                return false;
        }
        return true;
    }
    
    bool String::endsWith(String str) {
        const char *sbuf = str.strbuf;
        int len = str.length();
        
        for (int i=0; i<len; i++) {
            if (sbuf[i] != strbuf[i+(_length-len)])
                return false;
        }
        return true;
    }
    
    String &String::arg(String arg) {
        replace("%", arg, 1);
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
        
        int start_len = start.length();
        int s = indexOf(start, off);
        if (s != -1) {
            s += start_len;
            int l = indexOf(end, s);
            if (l == -1)
                l = _length - s;
            else
                l -= s;
            
            return substr(s, l);
        }
        
        return "";
    }
    
    String String::trim() {
        const char tchrs[] = " \t\r\n";
        
        //for (int i=0; i<4; i)
    }
    
};
