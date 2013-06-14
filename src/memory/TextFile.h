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

#include "Ref.h"

class TextFile : public File {
public:
    TextFile(const char *path) : File(path) {
        read_offset = 0;
    }
    
    Ref<const char> readLine() {
        unsigned int newline = read_offset;
    }

private:
    unsigned int read_offset;
};

#endif
