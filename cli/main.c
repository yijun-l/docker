#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"

#define VER "1.0"
#define DATE "12/26/2024"

char* program_name = "adocker";

void usage (int status){
    if (status != EXIT_SUCCESS){
        emit_try_help();
    }
    else{
        printf("\nUsage: %s [OPTION]...\n", program_name);
        fputs("\
\n\
    adocker is a client for interacting with the daemon through the CLI.\n\
\n\
        help        display this help and exit\n\
        version     output version information and exit\n\
\n\
", stdout);
    }

    exit (status);
}

void show_version(){
    printf("version %s, %s\n", VER, DATE);
}

int main(int argc, char* argv[]){

    if (argc != 2){
        usage(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "help") == 0){
        usage(EXIT_SUCCESS);
    } else if (strcmp(argv[1], "version") == 0){
        show_version();
    } else {
        usage(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}