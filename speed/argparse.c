//
// Created by Ghost on 2019/9/19.
//

#include "argparse.h"

static const OPTION *get_from_name(const OPTION *OPTION_ARRAY, int array_length, const char *str) {

    unsigned long i;
    const OPTION *current;

    for (i = 0; i < array_length; ++i) {

        current = &OPTION_ARRAY[i];
        if (!strncmp(str, current->name, strlen(current->name)) || str[0] == current->alias) {
            return current;
        }
    }
    return NULL;
}

static void get_value(int *index, const char *argv[], int count, const char **value) {

    int i;
    if (count == 0) return;

    for (i = 0; i < count; ++i) {
        (*index)++;
        value[i] = argv[*index];

    }
}

void parse_arg(const OPTION *options, int options_length,
               int argc, const char **argv,
               void *data, OPT_RESULT *opt) {

    int i;
    const OPTION *option;
    OPT_STATE state;
    const char **arg;

    if (options == NULL || options_length == 0) return;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            // the remain with no name, directly passed to the programme
            // return with valid offset for user to process the remaining data
            goto user_param;

        } else {
            option = get_from_name(options, options_length, argv[i][1] == '-' ? &argv[i][2] : &argv[i][1]);

            if (option == NULL) {
                opt->action = ACTION_ERR;
                opt->err_reason = "Invalid option";
                return;
            }

            // insufficient argument
            if (option->param_count + i >= argc) {
                opt->err_reason = "Insufficient arguments, settings should take a value.";
                opt->action = ACTION_ERR;
                return;
            }

            arg = malloc(sizeof(char *) * option->param_count);
            get_value(&i, argv, option->param_count, arg);

            state = option->callback(arg, option->param_count, data);
            free(arg);

            switch (state) {
                case CONTINUE:
                    break;

                case STOP:
                    goto user_param;

                case IGNORE:
                    goto finish;

                case ERR:
                    goto user_err;

                default:
                    break;
            }
        }
    }

    user_param:
    opt->offset = i;
    opt->action = ACTION_ADDITIONAL_PROCEED;
    opt->err_reason = "";
    return;

    finish:
    opt->action = ACTION_EXECUTE;
    opt->err_reason = "";
    opt->offset = argc;
    return;

    user_err:
    opt->action = ACTION_USER_ERR;
    opt->err_reason = "User parse function throws an error";
    opt->offset = i;
}

void print_arg_description(const OPTION *options, int options_length) {

    unsigned long i;
    OPTION current;

    for (i = 0; i < options_length; ++i) {

        current = options[i];
        printf("--%s %s, -%c %s\n"
               "\t\t\t\t\t\t\t\t%s\n", current.name, current.param_description,
               current.alias, current.param_description, current.description);

    }
}