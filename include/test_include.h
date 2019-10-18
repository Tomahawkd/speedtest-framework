//
// Created by Ghost on 2019/10/12.
//

#ifndef SPEEDTEST_TEST_INCLUDE_H
#define SPEEDTEST_TEST_INCLUDE_H

#include "speed.h"

static int test() {
    for (int i = 0; i < 1000; ++i) {
        int j = 1000 * 1000;
    }
    return 0;
}

#define ALGORITHM_COUNT 2
const ALGORITHM ALL_ALGORITHMS[ALGORITHM_COUNT] = {
        {"test1", "test1 test", ALGORITHM_SYM, test, NULL, NULL},
        {"test2", "test2 test", ALGORITHM_SYM, test, NULL, NULL},
};

#define ASYM_ALGORITHM_COUNT 1
const ASYM_ALGORITHM ALL_ASYM_ALGORITHMS[ASYM_ALGORITHM_COUNT] = {
        ASYM_ALGORITHM_new(test-asym, 512, ALGORITHM_WRP, test, NULL, NULL, test, NULL, NULL),
};

#endif //SPEEDTEST_TEST_INCLUDE_H
