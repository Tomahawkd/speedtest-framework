//
// Created by Ghost on 2019/9/19.
//

#ifndef TOOLS_ARGPARSE_H
#define TOOLS_ARGPARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum action {
    ACTION_EXECUTE,  // User can execute the programme and all of the args are processed
    ACTION_USER_ERR, // User parse function throws an error which should be stop immediately and process by user
    ACTION_ERR,   // param parse error, user receives err_reason and the param offset from OPT_RESULT
    ACTION_ADDITIONAL_PROCEED // additional param with no name should be passed directly to user programme
} ACTION;

typedef enum state {

    IGNORE,  // ignore the remain values
    CONTINUE, // continue to process the arg
    STOP, // stop process the arg and leave it to user programme
    ERR, // error occurs in the parse, this will lead a ACTION_USER_ERR
} OPT_STATE;

typedef struct {

    ACTION action;
    char *err_reason; // for action_err
    int offset; // there are more params passed directly to the programme which need to be proceed

} OPT_RESULT;

typedef struct {
    char *name; // full name of the option
    char alias; // short name of the option
    int param_count; // number of params option take
    char *param_description;
    char *description; // for help doc, what option does

    /* user callback for the specific param */

    // args are the values passed to the param, eg. -n 2 3 (2 and 3 passed to param n).

    // NOTICE: the args will be freed after executed the callback, users should not point to it
    // which could produces a wild pointer. But you can point to one of its element because the
    // element is from argv

    // count are the number of values passed to the param, for above example, the count is 2

    // user defined data for additional data to be passed or stored
    OPT_STATE (*callback)(const char **args, int count, void *data);

} OPTION;

/**
 *
 * @param argc arg count from main
 * @param argv arg variables from main
 * @param data additional data handled by user for transfer data between user program and callback,
 * @param opt parsed args value
 * this could be everything or a NULL
 */
void parse_arg(const OPTION *options, int options_length, int argc, const char **argv, void *data, OPT_RESULT *opt);

void print_arg_description(const OPTION *options, int options_length);

#endif //TOOLS_ARGPARSE_H
