//
//  Http.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 07/03/2013.
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

#ifndef HTTP_H_
#define HTTP_H_

#include <curl/curl.h>
#include <Memory/String.h>

namespace nrcore {

    class Http {
    public:
        Http();
        virtual ~Http();

        String get(const char* url);

    private:
        CURL *handle;
        String result;

        static size_t receive_cb(void *contents, size_t size, size_t nmemb, void *userp);
    };
    
};

#endif /* HTTP_H_ */
