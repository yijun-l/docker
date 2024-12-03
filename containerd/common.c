#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Print an error message and terminate the program
void sys_err(const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}