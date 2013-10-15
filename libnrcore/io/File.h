//
//  File.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 05/11/2012.
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

#ifndef PeerConnector_File_h
#define PeerConnector_File_h

#define FILE_BUFFER_SIZE        4096

#include <stdio.h>

#include <libnrcore/memory/Memory.h>

namespace nrcore {

    class File : public Memory {
    public:
        File(const char *path) : Memory(buffer, FILE_BUFFER_SIZE), fill(0), offset(0)  {
            fp = fopen(path, "r+");

            if (!fp)
                throw "File Not Found";

            fseek(fp, 0L, SEEK_END);
            sz = ftell(fp);
            fseek(fp, 0L, SEEK_SET);

            fill = fread(buffer, 1, FILE_BUFFER_SIZE, fp);
        }

        virtual ~File() {
            if (fp)
                fclose(fp);
        }

        char operator [](unsigned int index) {
            if (index>=sz)
                throw "Index Out Of Range";

            if (index >= offset && index < offset+fill)
                return Memory::operator [](index-((unsigned int)offset));

            offset = index-(index%FILE_BUFFER_SIZE);
            fseek(fp, offset, SEEK_SET);
            fill = fread(buffer, 1, FILE_BUFFER_SIZE, fp);
            return Memory::operator [](index-((unsigned int)offset));
        }

        Ref<char> getMemory() const {
            char *buf = new char[sz];
            fseek(fp, 0L, SEEK_SET);
            fread(buf, 1, sz, fp);
            return Ref<char>(buf, true);
        }

        size_t length() {
            return sz;
        }

    private:
        char buffer[FILE_BUFFER_SIZE];
        FILE* fp;
        size_t sz;
        size_t fill;

        size_t offset;
    };
    
};

#endif
