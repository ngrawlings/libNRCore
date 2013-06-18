//
//  StreamReader.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 22/05/2013.
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

#ifndef __PeerConnectorCore__StreamReader__
#define __PeerConnectorCore__StreamReader__

#include <libnrcore/threading/Task.h>
#include <libnrcore/memory/Stream.h>
#include <libnrcore/threading/Thread.h>

namespace nrcore {

    class StringStreamReader : public Task {
    public:
        StringStreamReader(Stream *stream);
        virtual ~StringStreamReader();
        
        void runBlockingMode();
        
        void close();
        
    protected:
        void run();
        virtual void onLineRead(const char* line) = 0;
        
    private:
        bool _run;
        
        Stream *stream;
        
        Thread *thread;
    };
    
};

#endif /* defined(__PeerConnectorCore__StreamReader__) */
