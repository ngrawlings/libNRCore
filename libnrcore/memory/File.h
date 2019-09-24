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
        File(const char *path) : Memory(FILE_BUFFER_SIZE), fill(0), offset(0), update_file(false)  {
            this->path = path;
            
            fp = fopen(path, "r+");
            if (!fp) {
                fp = fopen(path, "w+");
                if (!fp)
                    throw "Failed to open";
            }
            
            fseek(fp, 0L, SEEK_END);
            sz = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            
            fill = fread(buffer.getPtr(), 1, FILE_BUFFER_SIZE, fp);
        }
        
        virtual ~File() {
            if (update_file)
                updateFile();
            
            if (fp)
                fclose(fp);
        }
        
        char& operator [](size_t index) {
            if (index>=sz)
                throw "Index Out Of Range";
            
            if (fill && index >= offset && index < offset+fill)
                return Memory::operator [](index-offset);
            
            if (fill && update_file)
                updateFile();
            
            offset = index-(index%FILE_BUFFER_SIZE);
            fseek(fp, offset, SEEK_SET);
            fill = fread(buffer.getPtr(), 1, FILE_BUFFER_SIZE, fp);
            return Memory::operator [](index-offset);
        }
        
        
        Memory getMemory() const {
            char *buf = new char[sz];
            fseek(fp, 0L, SEEK_SET);
            size_t len = fread(buf, 1, sz, fp);
            
            Memory mem(buf, len);
            
            delete[] buf;
            
            return mem;
        }
        
        Memory getSubBytes(size_t offset, size_t length) const {
            char *buf = new char[length];
            fseek(fp, offset, SEEK_SET);
            length = fread(buf, 1, length, fp);
            
            Memory mem(buf, length);
            
            delete[] buf;
            
            return mem;
        }
        
        void write(size_t offset, const char* data, size_t length) {
            
            size_t len = 0;
            if (update_file) {
                if (offset < this->offset) { // copy preceeding bytes
                    len = (this->offset - offset > length) ? length : this->offset - offset;
                    writeToFile(offset, data, len);
                    offset += len;
                    data += len;
                    length -= len;
                }
                
                if (length) {
                    if (offset >= this->offset && offset < this->offset+fill) {
                        size_t coffset = this->offset - offset;
                        len = (fill-coffset) < length ? fill-coffset : length;
                        char *buf = &buffer.getPtr()[coffset];
                        memcpy(buf, data, len);
                        
                        offset += len;
                        data += len;
                        length -= len;
                    }
                }
                
                if (length)
                    writeToFile(offset, data, length);
                
            } else
                writeToFile(offset, data, length);
        }
        
        virtual size_t length() const {
            return sz;
        }
        
        void setFileUpdating(bool val) {
            update_file = val;
        }
        
        void grow(size_t size) {
            char byte = 0;
            fseek(fp, 0, SEEK_END);
            
            sz += size;
            
            while(size--)
                fwrite(&byte, 1, 1, fp);
        }
        
        void truncate() {
            if (fp) {
                offset = 0;
                fp = freopen(NULL, "w+", fp);
            }
        }
        
        int fileno() {
            return ::fileno(fp);
        }
        
    private:
        FILE* fp;
        size_t sz;
        size_t fill;
        
        size_t offset;
        
        String path;
        
        bool update_file;
        
        void updateFile(){
            fseek(fp, offset, SEEK_SET);
            size_t written = 0;
            while(written < fill)
                fwrite(&buffer.getPtr()[written], 1, fill, fp);
            fflush(fp);
        }
        
        void writeToFile(size_t offset, const char* data, size_t length) {
            fseek(fp, offset, SEEK_SET);
            size_t written = 0;
            while(written < length)
                written += fwrite(&data[written], 1, length, fp);
            fflush(fp);
        }
    };
    
};

#endif
