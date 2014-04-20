//
//  TextFile.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 07/06/2013.
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

#ifndef PeerConnectorCore_TextFile_h
#define PeerConnectorCore_TextFile_h

#include <libnrcore/base/Object.h>
#include <libnrcore/memory/Ref.h>
#include <libnrcore/memory/String.h>
#include "Stream.h"

#define MAX_LINE_LENGTH		4096

namespace nrcore {

    class TextStream : public Stream { //TODO class incomplete
    public:
       	TextStream(int fd) : Stream(fd) {
 
       	}
        
        String readLine() {
            char tmp[32];
            int pos = buffer.indexOf("\n");
            if (pos == -1) {
                do {
                    size_t len = read(tmp, 32);
                    buffer += String(tmp, len);
                    pos = buffer.indexOf("\n");
                } while (pos == -1);
            }
            
            String ret = buffer.substr(0, pos);
            buffer = buffer.substr(pos);
            
            return ret;
       	}

    private:
        String buffer;
    };
    
};

#endif
