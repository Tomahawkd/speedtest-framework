//
// Created by Ghost on 2019/10/18.
//

#ifndef CRYPTOLIB_SYS_RANDOM_H
#define CRYPTOLIB_SYS_RANDOM_H

#include <stdint.h>

void randseed(uint32_t seed);

void getrandombits(uint8_t *result, uint32_t length);

#endif //CRYPTOLIB_SYS_RANDOM_H
