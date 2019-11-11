//
// Created by Ghost on 2019/9/20.
//

#include "speedtester.h"

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {

    int (*test_func)(uint8_t *text, uint32_t textlen, uint8_t *out);

    struct timeval start;
    // time val to record the last time
    struct timeval last;

    uint8_t *text;
    uint32_t textlen;
    uint8_t *tmpout;
    long loop_count;

} SPEEDTEST_PARAM_RESULT;

static long double calc_time_interval(struct timeval *s, struct timeval *e) {
    return (e->tv_sec + (double) e->tv_usec / 1000000) - (s->tv_sec + (double) s->tv_usec / 1000000);
}

void non_thread_func(SPEEDTEST_PARAM_RESULT *param, OPT_CONF *option) {
    int err;

    gettimeofday(&param->start, NULL);
    for (;;) {
        err = param->test_func(param->text, param->textlen, param->tmpout);
        if (err) {
            printf("Error %d detect, abort\n", err);
            param->loop_count = -1;
            return;
        }

        gettimeofday(&param->last, NULL);
        param->loop_count++;

        if (calc_time_interval(&param->start, &param->last) >= option->interval) break;
    }
}

void thread_func(SPEEDTEST_PARAM_RESULT *param) {
    int err;

    gettimeofday(&param->start, NULL);
    for (;;) {
        err = param->test_func(param->text, param->textlen, param->tmpout);
        if (err) {
            printf("Error %d detect, abort\n", err);
            param->loop_count = -1;
            return;
        }

        gettimeofday(&param->last, NULL);
        param->loop_count++;

        // give a breakpoint that the cancel request can be proceeded
        pthread_testcancel();
    }
}

long double test_algorithm(const ALGORITHM *algorithm, OPT_CONF *options, uint8_t *text, const uint32_t textlen, uint8_t *tmpout) {

    long double interval;
    long double speed;
    long sum_loop = 0;
    int i, num = (options->threads > 0 ? options->threads : 1);

    pthread_t *pthreads = NULL;
    struct timeval threads_s, threads_t;
    struct timespec req = {options->interval - 1, 999900000}, rem;
    SPEEDTEST_PARAM_RESULT *params = malloc(sizeof(SPEEDTEST_PARAM_RESULT) * num);
    memset(params, 0, sizeof(SPEEDTEST_PARAM_RESULT) * options->threads);
    for (i = 0; i < num; ++i) {
        params[i].test_func = algorithm->test_func;
        params[i].text = text;
        params[i].textlen = textlen;
        params[i].tmpout = tmpout;
    }

    if (algorithm->setup_func) algorithm->setup_func();
    printf("testing %s on %d bytes size: ", algorithm->description, textlen);
    fflush(stdout);
    if (options->threads > 1) printf("\n");

    if (options->threads > 0) {

        pthreads = malloc(sizeof(pthread_t) * options->threads);

        gettimeofday(&threads_s, NULL);
        for (i = 0; i < options->threads; ++i) {
            if (pthread_create(&pthreads[i], NULL, (void *) thread_func, &params[i])) {
                printf("Thread %d creation error, abort\n", i + 1);
                for (; i >= 0; --i) {
                    pthread_cancel(pthreads[i]);
                    pthread_join(pthreads[i], NULL);
                }
                speed = -2;
                goto cleanup;

            } else {
                if (options->threads > 1)
                    printf("Thread %d started\n", i + 1);
            }
        }

        nanosleep(&req, &rem);

        for (i = 0; i < options->threads; ++i) pthread_cancel(pthreads[i]);
        for (i = 0; i < options->threads; ++i) pthread_join(pthreads[i], NULL);
        gettimeofday(&threads_t, NULL);

        if (options->threads > 1) {
            for (i = 0; i < options->threads; ++i) {
                if (params[i].loop_count == -1) {
                    speed = -2;
                    goto cleanup;
                }
                interval = calc_time_interval(&params[i].start, &params[i].last);
                speed = (long double) params[i].loop_count / interval;
                printf("Thread %d: %ld loops used %Lf s. (Approximate %Lf tps)\n",
                       i + 1, params[i].loop_count, interval, speed);
                sum_loop += params[i].loop_count;
            }

            interval = calc_time_interval(&threads_s, &threads_t);
            speed = (long double) sum_loop / interval;
            printf("%ld loops used %Lf s. (Approximate %Lf tps)\n",
                   sum_loop, interval, speed);

            goto cleanup;
        } else {
            goto single_test_result;
        }
    } else {
        non_thread_func(&params[0], options);
        if (params[i].loop_count == -1) {
            speed = -2;
            goto cleanup;
        }
    }

    single_test_result:
    interval = calc_time_interval(&params[0].start, &params[0].last);
    speed = (long double) params[0].loop_count / interval;
    printf("%ld loops used %Lf s. (Approximate %Lf tps)\n",
           params[0].loop_count, interval, speed);

    cleanup:
    if (algorithm->cleanup) algorithm->cleanup();
    free(params);
    if (pthreads != NULL) free(pthreads);
    return speed;
}
