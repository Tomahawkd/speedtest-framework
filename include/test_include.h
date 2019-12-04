//
// Created by Ghost on 2019/10/12.
//

#ifndef SPEEDTEST_TEST_INCLUDE_H
#define SPEEDTEST_TEST_INCLUDE_H

#include "speed.h"
#include "test/aes_test.h"

#define ALGORITHM_COUNT 12
const ALGORITHM ALL_ALGORITHMS[ALGORITHM_COUNT] = {
        {"aes-128-ecb", "AES-128-ecb", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_128_ecb, cleanup_wrp_aes},
        {"aes-192-ecb", "AES-192-ecb", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_192_ecb, cleanup_wrp_aes},
        {"aes-256-ecb", "AES-256-ecb", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_256_ecb, cleanup_wrp_aes},
        {"aes-128-cbc", "AES-128-cbc", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_128_cbc, cleanup_wrp_aes},
        {"aes-192-cbc", "AES-192-cbc", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_192_cbc, cleanup_wrp_aes},
        {"aes-256-cbc", "AES-256-cbc", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_256_cbc, cleanup_wrp_aes},
        {"aes-128-ctr", "AES-128-ctr", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_128_ctr, cleanup_wrp_aes},
        {"aes-192-ctr", "AES-192-ctr", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_192_ctr, cleanup_wrp_aes},
        {"aes-256-ctr", "AES-256-ctr", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_256_ctr, cleanup_wrp_aes},
        {"aes-128-gcm", "AES-128-gcm", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_128_gcm, cleanup_wrp_aes},
        {"aes-192-gcm", "AES-192-gcm", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_192_gcm, cleanup_wrp_aes},
        {"aes-256-gcm", "AES-256-gcm", ALGORITHM_SYM, test_wrp_aes, setup_wrp_aes_256_gcm, cleanup_wrp_aes},
};

#define ASYM_ALGORITHM_COUNT 0
const ASYM_ALGORITHM ALL_ASYM_ALGORITHMS[ASYM_ALGORITHM_COUNT] = {

};

#define KEYGEN_ALGORITHMS_COUNT 0
const ALGORITHM KEYGEN_ALGORITHMS[KEYGEN_ALGORITHMS_COUNT] = {
};

#endif //SPEEDTEST_TEST_INCLUDE_H
