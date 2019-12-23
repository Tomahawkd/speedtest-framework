//
// Created by Ghost on 2019/9/20.
//

#include "speedtester.h"

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <pthread.h>
#endif

#ifdef _WIN32

static BOOL timeup = FALSE;

// reference from: https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
// the struct already defined in winsocks.h in my pc, uncomment the follows if not defined
//typedef struct timeval {
//    long tv_sec;
//    long tv_usec;
//};

int gettimeofday(struct timeval * tp, struct timezone * tzp) {
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32u;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#endif

typedef struct {

    int (*test_func)(void *ctx, uint8_t *text, uint32_t textlen, uint8_t *out);

    struct timeval start;
    // time val to record the last time
    struct timeval last;

    uint8_t *text;
    uint32_t textlen;
    uint8_t *tmpout;
    long loop_count;
    void *ctx;

} SPEEDTEST_PARAM_RESULT;

static long double calc_time_interval(struct timeval *s, struct timeval *e) {
    return (e->tv_sec + (double) e->tv_usec / 1000000) - (s->tv_sec + (double) s->tv_usec / 1000000);
}

void non_thread_func(SPEEDTEST_PARAM_RESULT *param, OPT_CONF *option) {

    gettimeofday(&param->start, NULL);
    for (;;) {
        int err = param->test_func(param->ctx, param->text, param->textlen, param->tmpout);
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

    gettimeofday(&param->start, NULL);
    gettimeofday(&param->last, NULL);
    for (;;) {
        int err = param->test_func(param->ctx, param->text, param->textlen, param->tmpout);
        if (err) {
            printf("Error 0x%.2X detect at %ld's loop, abort\n", err, param->loop_count);
            param->loop_count = -1;
            return;
        }

        gettimeofday(&param->last, NULL);
        param->loop_count++;

#ifndef _WIN32
        // give a breakpoint that the cancel request can be proceeded
        pthread_testcancel();
#else
        if (timeup) ExitThread(0);
#endif
    }
}

long double test_algorithm(const ALGORITHM *algorithm, OPT_CONF *options, uint8_t *text, uint32_t textlen, uint8_t *tmpout) {

    long double interval;
    long double speed;
    long sum_loop = 0;
    int i, num = (options->threads > 0 ? options->threads : 1);

#ifdef _WIN32
    HANDLE *pthreads = NULL;
#else
    pthread_t *pthreads = NULL;
    struct timespec req = {options->interval - 1, 999900000}, rem;
#endif
    struct timeval threads_s, threads_t;
    SPEEDTEST_PARAM_RESULT *params = malloc(sizeof(SPEEDTEST_PARAM_RESULT) * num);
    memset(params, 0, sizeof(SPEEDTEST_PARAM_RESULT) * options->threads);
    for (i = 0; i < num; ++i) {
        params[i].test_func = algorithm->test_func;
        params[i].text = text;
        params[i].textlen = textlen;
        params[i].tmpout = tmpout;
        if (algorithm->setup_func) algorithm->setup_func(&params[i].ctx);
    }

    printf("testing %s on %u bytes size: ", algorithm->description, textlen);
    fflush(stdout);
    if (options->threads > 1) printf("\n");

    if (options->threads > 0) {

#ifdef _WIN32
        pthreads = malloc(sizeof(HANDLE) * options->threads);
        timeup = FALSE;
#else
        pthreads = malloc(sizeof(pthread_t) * options->threads);
#endif

        gettimeofday(&threads_s, NULL);
        for (i = 0; i < options->threads; ++i) {
#ifdef _WIN32
            pthreads[i] = CreateThread(NULL, 0,
                    (LPTHREAD_START_ROUTINE) thread_func, &params[i], 0, NULL);
            if (pthreads[i] == NULL) {
                printf("Thread %d creation error, abort\n", i + 1);
                fflush(stdout);
                for (; i >= 0; --i) {
                    TerminateThread(pthreads[i], 0);
                }
#else
            if (pthread_create(&pthreads[i], NULL, (void *) thread_func, &params[i])) {
                printf("Thread %d creation error, abort\n", i + 1);
                fflush(stdout);
                for (; i >= 0; --i) {
                    pthread_cancel(pthreads[i]);
                    pthread_join(pthreads[i], NULL);
                }
#endif
                speed = -2;
                goto cleanup;

            } else {
                if (options->threads > 1)
                    printf("Thread %d started\n", i + 1);
                fflush(stdout);
            }
        }

#ifdef _WIN32
        Sleep(options->interval * 1000);
        timeup = TRUE;
        for (i = 0; i < options->threads; ++i) WaitForSingleObject(pthreads[i], INFINITE);
#else
        nanosleep(&req, &rem);
        for (i = 0; i < options->threads; ++i) pthread_cancel(pthreads[i]);
        for (i = 0; i < options->threads; ++i) pthread_join(pthreads[i], NULL);
#endif
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
                fflush(stdout);
                sum_loop += params[i].loop_count;
            }

            interval = calc_time_interval(&threads_s, &threads_t);
            speed = (long double) sum_loop / interval;
            printf("%ld loops used %Lf s. (Approximate %Lf tps)\n",
                   sum_loop, interval, speed);
            fflush(stdout);

            goto cleanup;
        } else {
            if (params[0].loop_count == -1) {
                speed = -2;
                goto cleanup;
            }
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
    fflush(stdout);

cleanup:
    if (algorithm->cleanup)
        for (i = 0; i < num; ++i) algorithm->cleanup(&params[i].ctx);
    free(params);
    if (pthreads != NULL) free(pthreads);
    return speed;
}
