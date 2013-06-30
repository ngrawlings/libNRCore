//
//  Http.cpp
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

#include "Http.h"

namespace nrcore {

    Http::Http() {
#ifdef WITH_LIBCURL
        handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, receive_cb);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);
#endif
    }

    Http::~Http() {
#ifdef WITH_LIBCURL
        curl_easy_cleanup(handle);
#endif
    }

    String Http::get(const char* url) {
        result = "";
#ifdef WITH_LIBCURL
        curl_easy_setopt(handle, CURLOPT_URL, "https://android.googleapis.com/gcm/send");
        curl_easy_perform(handle);
#endif
        return result;
    }

    size_t Http::receive_cb(void *contents, size_t size, size_t nmemb, void *inst) {
        size_t realsize = size * nmemb;
        dynamic_cast<Http*>((Http*)inst)->result += String((char*)contents);
        return realsize;
    }

};