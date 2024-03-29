//
// Created by Ghost on 2019/10/18.
//

#include "sys_random.h"
#include <stdlib.h>
#ifdef _WIN32
#include <time.h>
#endif

void randseed(uint32_t seed) {
#ifndef __APPLE__
# ifdef _WIN32
    srand(seed);
#else
    srandom(seed);
# endif
#endif
}

void getrandombits(uint8_t *result, uint32_t length) {

#ifdef __APPLE__
    arc4random_buf(result, length);
#else
    uint32_t offset = 0;

    while(offset < length) {
# ifdef _WIN32
        result[offset] = rand();
# else
        result[offset] = random();
# endif
        offset++;
    }
#endif
}