//
//  Buffer.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 14/05/2013.
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

#ifndef __PeerConnectorCore__Buffer__
#define __PeerConnectorCore__Buffer__

#include <libnrcore/memory/Memory.h>

namespace nrcore {

    class Buffer {
    public:
        Buffer(int flags, int len);
        virtual ~Buffer();
        
        Memory* buffer();
        size_t fill();
        size_t fill(size_t new_fill);
        void incrementFill(size_t inc);
        char    flags();
        char    flags(char new_flags);
        
    private:
        Memory  *_buffer;
        size_t  _fill;
        char    _flags;
    };
    
};

#endif /* defined(__PeerConnectorCore__Buffer__) */
