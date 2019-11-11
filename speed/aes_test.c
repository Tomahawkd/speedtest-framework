//
// Created by Ghost on 2019/10/16.
//

#include "test/aes_test.h"
#include <openssl/evp.h>
#include <string.h>

static uint8_t k[32];
static uint8_t iv[32];
static uint8_t tmpiv[32];
static uint8_t aad[32];

static EVP_CIPHER_CTX *ctx;

#define AES_TESTSUITE(keylen, mode) \
void setup_wrp_aes_##keylen##_##mode() { \
    ctx = EVP_CIPHER_CTX_new(); \
    EVP_EncryptInit_ex(ctx, EVP_aes_##keylen##_##mode(), NULL, k, iv); \
    EVP_CIPHER_CTX_set_padding(ctx, 0); \
} \

#define AES_GCM(keylen) \
void setup_wrp_aes_##keylen##_gcm() { \
    int len = 0; \
    ctx = EVP_CIPHER_CTX_new(); \
    EVP_CIPHER_CTX_set_padding(ctx, 0); \
    EVP_EncryptInit_ex(ctx, EVP_aes_##keylen##_gcm(), NULL, NULL, NULL); \
    EVP_EncryptInit_ex(ctx, NULL, NULL, k, iv); \
    EVP_EncryptUpdate(ctx, NULL, &len, aad, 32); \
}

int test_wrp_aes(uint8_t *text, uint32_t textlen, uint8_t *tmpout) {
    int32_t outlen = 0;
    if (!EVP_EncryptUpdate(ctx, tmpout, &outlen, text, textlen)) return 1;
    return 0;
}

void cleanup_wrp_aes() {
    EVP_CIPHER_CTX_free(ctx);
}

AES_TESTSUITE(128, ecb)
AES_TESTSUITE(128, cbc)
AES_TESTSUITE(128, ctr)
AES_GCM(128)
AES_TESTSUITE(192, ecb)
AES_TESTSUITE(192, cbc)
AES_TESTSUITE(192, ctr)
AES_GCM(192)
AES_TESTSUITE(256, ecb)
AES_TESTSUITE(256, cbc)
AES_TESTSUITE(256, ctr)
AES_GCM(256)
