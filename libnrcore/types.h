//
//  types.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 10/16/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef libNRCore_types_h
#define libNRCore_types_h

#include <libnrcore/config.h>

#ifndef WIN32

#include <unistd.h>

#define thread_t        pthread_t
#define thread_mutex_t  pthread_mutex_t
#define thread_cond_t   pthread_cond_t
#define thread_key_t    pthread_key_t

#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SSIZE_T         ssize_t
#define thread_t        HANDLE
#define thread_mutex_t  HANDLE
#define thread_cond_t   HANDLE
#define thread_key_t    HANDLE

#endif

#endif
