//
//  ObjectArray.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 04/03/2013.
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

#ifndef __PeerConnector__ObjectArray__
#define __PeerConnector__ObjectArray__

template <class T>
class ObjectArray {
public:
    explicit ObjectArray<T>(int cnt) {
        this.cnt = cnt;
        objs = new T[cnt];
    }
    
    virtual ~ObjectArray<T> () {
        delete [] objs;
    }
    
    T* get(int index) {
        return objs[index];
    }
    
    int length() {
        return cnt;
    }

private:
    T *objs;
    int cnt;
};

#endif /* defined(__PeerConnector__ObjectArray__) */
