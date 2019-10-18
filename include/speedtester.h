//
// Created by Ghost on 2019/9/20.
//

#ifndef SPEEDTEST_SPEEDTESTER_H
#define SPEEDTEST_SPEEDTESTER_H

#include <stdio.h>
#include <stdint.h>
#include "speed.h"

long double test_algorithm(const ALGORITHM *algorithm, OPT_CONF *options, uint8_t *text, uint32_t textlen, uint8_t *tmpout);

#endif //SPEEDTEST_SPEEDTESTER_H
