//
//  base58.hpp
//  libnrcore
//
//  Created by Nyhl Rawlings on 17/10/2019.
//  Copyright © 2019 N G Rawlings. All rights reserved.
//

#ifndef base58_hpp
#define base58_hpp

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool (*b58_sha256_impl)(void *, const void *, size_t);

extern bool b58tobin(void *bin, size_t *binsz, const char *b58, size_t b58sz);
extern int b58check(const void *bin, size_t binsz, const char *b58, size_t b58sz);

extern bool b58enc(char *b58, size_t *b58sz, const void *bin, size_t binsz);

#ifdef __cplusplus
}

#endif


#endif /* base58_hpp */
