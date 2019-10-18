//
// Created by Ghost on 2019/9/19.
//

#include <stdio.h>
#include <math.h>
#include "sys_random.h"

#ifdef _WIN32
#else
#include <sys/utsname.h>
#endif

#include "speed.h"
#include "argparse.h"
#include "speedtester.h"
#include "test_include.h"

//======================================== argument process definition ===================================//
static OPT_STATE __help(const char **args, int count, void *data);

static OPT_STATE __target_symmetric(const char **args, int count, void *data);

static OPT_STATE __target_asymmetric(const char **args, int count, void *data);

static OPT_STATE __target_whitebox(const char **args, int count, void *data);

static OPT_STATE __target_hash(const char **args, int count, void *data);

static OPT_STATE __target_hmac(const char **args, int count, void *data);

static OPT_STATE __target_wrp(const char **args, int count, void *data);

static OPT_STATE __list(const char **args, int count, void *data);

static OPT_STATE __thread_set(const char **args, int count, void *data);

static OPT_STATE __seconds_set(const char **args, int count, void *data);

//============================================== OPTIONS ==================================================//

#define OPTION_ARRAY_LENGTH 10
const OPTION OPTION_ARRAY[OPTION_ARRAY_LENGTH] = {
        {"help",       'h', 0, "",             "Display this summary",                               __help},
        {"symmetric",  'S', 0, "",             "Test symmetric ciphers",                             __target_symmetric},
        {"asymmetric", 'A', 0, "",             "Test asymmetric ciphers",                            __target_asymmetric},
        {"whitebox",   'W', 0, "",             "Test whitebox ciphers",                              __target_whitebox},
        {"hash",       'H', 0, "",             "Test hash functions",                                __target_hash},
        {"hmac",       'M', 0, "",             "Test hmac functions",                                __target_hmac},
        {"wrp",        'w', 0, "",             "Test wrp algorithms",                                __target_wrp},
        {"list",       'l', 0, "",             "List avaliable crypto algorithms",                   __list},
        {"thread",     't', 1, "<thread num>", "set num of thread to run, set 0 for not use thread", __thread_set},
        {"seconds",    's', 1, "<interval>",   "Set time interval for algorithm to loop",            __seconds_set},
};

// ============================================ Helping functions ========================================//
static void print_help_str() {
    printf("Usage: speed [options] [ciphers..]\n");
    printf("Valid options are:\n");
    print_arg_description(OPTION_ARRAY, OPTION_ARRAY_LENGTH);
}

static void list_algorithms() {

    unsigned long i;
    ALGORITHM current;
    ASYM_ALGORITHM cu;
    printf("Available algorithms:\n");

    for (i = 0; i < ALGORITHM_COUNT; ++i) {
        current = ALL_ALGORITHMS[i];
        printf("%s ", current.name);
    }

    for (i = 0; i < ASYM_ALGORITHM_COUNT; ++i) {
        cu = ALL_ASYM_ALGORITHMS[i];
        printf("%s ", cu.name);
    }
}

#define CONVERT_PUB 0
#define CONVERT_PRI 1
static ALGORITHM convert_algo;

static ALGORITHM *convert(const ASYM_ALGORITHM *asym, int type) {

    convert_algo.type = ALGORITHM_ASYM;
    convert_algo.name = asym->name;

    if (type == CONVERT_PUB) {
        convert_algo.description = asym->pub_description;
        convert_algo.test_func = asym->pub_test_func;
        convert_algo.setup_func = asym->pub_setup_func;
        convert_algo.cleanup = asym->pub_cleanup;
    } else {
        convert_algo.description = asym->pri_description;
        convert_algo.test_func = asym->pri_test_func;
        convert_algo.setup_func = asym->pri_setup_func;
        convert_algo.cleanup = asym->pri_cleanup;
    }

    return &convert_algo;
}

#define MAX_TEST_SIZE 16384
static uint8_t text[MAX_TEST_SIZE] = {0};
static uint8_t tmpout[MAX_TEST_SIZE + 1024] = {0};

#define TEST_SIZE_COUNT 6
static uint32_t text_size_arr[TEST_SIZE_COUNT] = {16, 64, 256, 1024, 8192, 16384};

#define NOT_TEST 0
#define TEST_ERROR 1
#define TO_TEST 2
static int speed_flag[ALGORITHM_COUNT] = {NOT_TEST};
static int asym_speed_flag[ASYM_ALGORITHM_COUNT] = {NOT_TEST};
static long double speed_result[ALGORITHM_COUNT][TEST_SIZE_COUNT] = {0};
static long double asym_speed_result[ASYM_ALGORITHM_COUNT][2] = {0};

int main(int argc, const char *argv[]) {

    unsigned long i;
    int j, k;
    int is_valid = 0, is_ciph = 0, is_asym = 0;
    long double speed;

    getrandombits(text, MAX_TEST_SIZE);

    // default settings
    OPT_CONF opt = {EXECUTE_MODE, ALGORITHM_DEFAULT, 3, 1};
    OPT_RESULT result;

#ifdef _WIN32
#else
    struct utsname n;
#endif

    parse_arg(OPTION_ARRAY, OPTION_ARRAY_LENGTH, argc, argv, &opt, &result);

    switch (result.action) {

        case ACTION_ERR:
            printf("%s\n", result.err_reason);
            print_help_str();
            return 1;

        case ACTION_USER_ERR:
            goto user_err;

        case ACTION_EXECUTE:
        case ACTION_ADDITIONAL_PROCEED:
            if (opt.mask == ALGORITHM_DEFAULT) opt.mask = ALGORITHM_ALL;
            goto execute;
    }

    execute:
    if (opt.mode == EXECUTE_MODE) {

        // pre-setup test target
        if (result.offset >= argc) {
            for (i = 0; i < ALGORITHM_COUNT; ++i) {
                if (opt.mask & ALL_ALGORITHMS[i].type) {
                    speed_flag[i] = TO_TEST;
                    is_ciph = 1;
                }
            }

            if (opt.mask & ALGORITHM_ASYM) {
                for (i = 0; i < ASYM_ALGORITHM_COUNT; ++i) {
                    asym_speed_flag[i] = TO_TEST;
                    is_asym = 1;
                }
            }

        } else {

            is_valid = 0;
            for (j = result.offset; j < argc; ++j) {

                for (i = 0; i < ALGORITHM_COUNT; ++i) {
                    if (opt.mask & ALL_ALGORITHMS[i].type &&
                    !strncmp(argv[j], ALL_ALGORITHMS[i].name, strlen(argv[j]))) {
                        speed_flag[i] = TO_TEST;
                        is_valid = 1;
                        is_ciph = 1;
                    }
                }

                if (is_valid) continue;

                for (i = 0; i < ASYM_ALGORITHM_COUNT; ++i) {
                    if (opt.mask & ALL_ASYM_ALGORITHMS[i].type &&
                        !strncmp(argv[j], ALL_ASYM_ALGORITHMS[i].name, strlen(argv[j]))) {
                        asym_speed_flag[i] = TO_TEST;
                        is_valid = 1;
                        is_asym = 1;
                    }
                }


                if (is_valid) continue;
                printf("Algorithm not found, abort.");
                goto user_err;
            }
        }

        for (i = 0; i < ALGORITHM_COUNT; ++i) {
            if (speed_flag[i] == TO_TEST) {
                for (k = 0; k < TEST_SIZE_COUNT; ++k) {
                    speed = test_algorithm(&ALL_ALGORITHMS[i], &opt, text, text_size_arr[k], tmpout);
                    if (speed < 0) speed_flag[i] = TEST_ERROR;
                    else speed_result[i][k] = speed * 8 * text_size_arr[k] / 1000 / 1000;
                }
            }
        }

        for (i = 0; i < ASYM_ALGORITHM_COUNT; ++i) {
            if (asym_speed_flag[i] == TO_TEST) {
                speed = test_algorithm(convert(&ALL_ASYM_ALGORITHMS[i], CONVERT_PUB), &opt, text, ALL_ASYM_ALGORITHMS[i].keylen, tmpout);
                if (speed < 0) asym_speed_flag[i] = TEST_ERROR;
                else asym_speed_result[i][CONVERT_PUB] = speed;

                speed = test_algorithm(convert(&ALL_ASYM_ALGORITHMS[i], CONVERT_PRI), &opt, text, ALL_ASYM_ALGORITHMS[i].keylen, tmpout);
                if (speed < 0) asym_speed_flag[i] = TEST_ERROR;
                else asym_speed_result[i][CONVERT_PRI] = speed;
            }
        }

#ifdef _WIN32
#else
        uname(&n);
        printf("\nSystem info:\n");
        printf("System name: %s\n", n.sysname);
        printf("System version: %s\n", n.version);
        printf("Hardware type: %s\n", n.machine);
#endif

        // summary
        if (is_ciph) {
            printf("alg_name%10.s\t", "");
            for (k = 0; k < TEST_SIZE_COUNT; ++k) {
                printf("%d bytes    \t", text_size_arr[k]);
            }

            printf("\n");
            for (i = 0; i < ALGORITHM_COUNT; ++i) {
                if (speed_flag[i] == NOT_TEST) continue;

                printf("%s%*.s\t", ALL_ALGORITHMS[i].description,
                       (int) (18 < strlen(ALL_ALGORITHMS[i].description) ?
                              strlen(ALL_ALGORITHMS[i].description) : 18 - strlen(ALL_ALGORITHMS[i].description)),
                       "");

                if (speed_flag[i] == TEST_ERROR) {
                    printf("Algorithm error at runtime.\n");
                    continue;
                }
                for (k = 0; k < TEST_SIZE_COUNT; ++k) {
                    printf("%6.*Lf Mbps\t", (6 - (int) log10l(speed_result[i][k])), speed_result[i][k]);
                }
                printf("\n");
            }
        }

        if (is_asym) {
            printf("alg_name%10.s\t", "");
            printf("sign/s        \t");
            printf("verify/s\n");

            for (i = 0; i < ASYM_ALGORITHM_COUNT; ++i) {
                if (asym_speed_flag[i] == NOT_TEST) continue;

                printf("%s%*.s\t", ALL_ASYM_ALGORITHMS[i].name,
                       (int) (18 < strlen(ALL_ASYM_ALGORITHMS[i].name) ?
                              strlen(ALL_ASYM_ALGORITHMS[i].name) : 18 - strlen(ALL_ASYM_ALGORITHMS[i].name)),
                       "");

                if (asym_speed_flag[i] == TEST_ERROR) {
                    printf("Algorithm error at runtime.\n");
                    continue;
                }

                printf("%6.*Lf tbps\t", (6 - (int) log10l(asym_speed_result[i][CONVERT_PRI])),
                       asym_speed_result[i][CONVERT_PRI]);
                printf("%6.*Lf tbps\t", (6 - (int) log10l(asym_speed_result[i][CONVERT_PUB])),
                       asym_speed_result[i][CONVERT_PUB]);
                printf("\n");
            }
        }

        if (!is_asym && !is_ciph) {
            printf("No algorithm selected.\n");
        }
    }

    return 0;

    user_err:
    return 1;
}

//======================================= argument process implementation =====================================//
static OPT_STATE __help(const char **args, int count, void *data) {
    print_help_str();
    ((OPT_CONF *) data)->mode = HELP_MODE;
    return IGNORE;
}

static OPT_STATE __target_symmetric(const char **args, int count, void *data) {
    ((OPT_CONF *) data)->mask |= ALGORITHM_SYM;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __target_asymmetric(const char **args, int count, void *data) {
    ((OPT_CONF *) data)->mask |= ALGORITHM_ASYM;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __target_whitebox(const char **args, int count, void *data) {
    ((OPT_CONF *) data)->mask |= ALGORITHM_WHITEBOX;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __target_hash(const char **args, int count, void *data) {
    ((OPT_CONF *) data)->mask |= ALGORITHM_HASH;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __target_hmac(const char **args, int count, void *data) {
    ((OPT_CONF *) data)->mask |= ALGORITHM_HMAC;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __target_wrp(const char **args, int count, void *data) {
    ((OPT_CONF *) data)->mask |= ALGORITHM_WRP;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __list(const char **args, int count, void *data) {
    list_algorithms();
    ((OPT_CONF *) data)->mode = HELP_MODE;
    return IGNORE;
}

static OPT_STATE __thread_set(const char **args, int count, void *data) {

    int intval = atoi(args[0]);

    // int parse error or minus digit got
    if (intval < 0 || (intval == 0 && args[0][0] != '0' && strlen(args[0]) != 1)) {
        printf("Invalid thread number %s\n", args[0]);
        return ERR;
    }

    ((OPT_CONF *) data)->threads = intval;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}

static OPT_STATE __seconds_set(const char **args, int count, void *data) {

    int intval = atoi(args[0]);

    // int parse error or minus digit got
    if (intval <= 0) {
        printf("Invalid seconds %s\n", args[0]);
        return ERR;
    }

    ((OPT_CONF *) data)->interval = intval;
    ((OPT_CONF *) data)->mode = EXECUTE_MODE;

    return CONTINUE;
}