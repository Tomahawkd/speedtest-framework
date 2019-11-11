//
// Created by Ghost on 2019/10/18.
//

#ifndef SPEEDTEST_SYS_RANDOM_H
#define SPEEDTEST_SYS_RANDOM_H

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

void randseed(uint32_t seed);

void getrandombits(uint8_t *result, uint32_t length);

#ifdef  __cplusplus
}
#endif

#endif //SPEEDTEST_SYS_RANDOM_H
