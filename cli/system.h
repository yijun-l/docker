#ifndef SYSTEM_H
#define SYSTEM_H

#define emit_try_help() \
    do \
        { \
            fprintf(stderr, "Try '%s help' for more information.\n", program_name); \
        } \
    while (0)

#endif
