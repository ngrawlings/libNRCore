//
//  types.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 10/16/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef libNRCore_types_h
#define libNRCore_types_h

#include "../config.h"

#ifndef WIN32

#include <unistd.h>

#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SSIZE_T ssize_t

#endif

#endif
