#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

// Print an error message and terminate the program
extern void sys_err(const char* msg);

// Change the hostname and print the old and new values
extern void ch_hostname(const char* new_hostname);

// Map uid/gid
extern void setup_user_namespace();

#define DEBUG 1

#define info(msg, ...) do{ \
    if(DEBUG){ \
        printf("[%s:%d >> %s()] " msg"", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    } \
}while(0)

#endif