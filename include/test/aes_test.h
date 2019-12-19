//
// Created by Ghost on 2019/10/16.
//

#ifndef SPEEDTEST_AES_TEST_H
#define SPEEDTEST_AES_TEST_H

#include <stdint.h>

#define AES_TESTSUITE_HEADER(keylen, mode) \
void setup_wrp_aes_##keylen##_##mode(void **ctx);

int test_wrp_aes(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *tmpout);
void cleanup_wrp_aes(void **ctx);

AES_TESTSUITE_HEADER(128, ecb)
AES_TESTSUITE_HEADER(128, cbc)
AES_TESTSUITE_HEADER(128, ctr)
AES_TESTSUITE_HEADER(128, gcm)
AES_TESTSUITE_HEADER(192, ecb)
AES_TESTSUITE_HEADER(192, cbc)
AES_TESTSUITE_HEADER(192, ctr)
AES_TESTSUITE_HEADER(192, gcm)
AES_TESTSUITE_HEADER(256, ecb)
AES_TESTSUITE_HEADER(256, cbc)
AES_TESTSUITE_HEADER(256, ctr)
AES_TESTSUITE_HEADER(256, gcm)

#endif //SPEEDTEST_AES_TEST_H
