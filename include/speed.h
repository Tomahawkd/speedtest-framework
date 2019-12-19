//
// Created by Ghost on 2019/9/20.
//

#ifndef SPEEDTEST_SPEED_H
#define SPEEDTEST_SPEED_H

#include "argparse.h"
#include <stdint.h>

//========================== ALGORITHM Definitions ===============================//

#define ALGORITHM_SYM       0b000001u
#define ALGORITHM_ASYM      0b000010u
#define ALGORITHM_HASH      0b000100u
#define ALGORITHM_HMAC      0b001000u
#define ALGORITHM_WHITEBOX  0b010000u
#define ALGORITHM_KEYGEN    0b100000u
#define ALGORITHM_ALL       0b111111u
// this value is only to identify if it is default, the mask will be set to ALL if DEFAULT
#define ALGORITHM_DEFAULT   0b000000u

typedef struct {

    char *name; // algorithm name
    char *description;
    unsigned int type;  // algorithm type
    int (*test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out); // algorithm test function
    void (*setup_func)(void **ctx); // setup functions like keygen
    void (*cleanup)(void **ctx); // cleanup function

} ALGORITHM;

#define QUOTE(str) #str
#define ASYM_ALGORITHM_new(name, NAME, keysize, type, pub_test_func, pri_test_func, setup_func, cleanup) \
{QUOTE(name-keysize), keysize/8, type | ALGORITHM_ASYM, QUOTE(NAME-keysize public operation), QUOTE(NAME-keysize private operation), pub_test_func, setup_func, cleanup, pri_test_func, setup_func, cleanup}

typedef struct {

    char *name; // algorithm names
    uint32_t keylen;
    unsigned int type;
    char *pub_description;
    char *pri_description;

    int (*pub_test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out); // algorithm test function
    void (*pub_setup_func)(void **ctx); // setup functions like keygen
    void (*pub_cleanup)(void **ctx); // cleanup function
    int (*pri_test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out); // algorithm test function
    void (*pri_setup_func)(void **ctx); // setup functions like keygen
    void (*pri_cleanup)(void **ctx); // cleanup function

} ASYM_ALGORITHM;

//========================== User defined data Definitions ===============================//

#define HELP_MODE 0
#define EXECUTE_MODE 1

typedef struct {

    /* mode */
    int mode;

    /* Test target */
    unsigned int mask; // Algorithm type to test

    /* Settings */
    int interval; // time interval for test loop
    int threads; // threads count

} OPT_CONF;

#endif //SPEEDTEST_SPEED_H
