#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>

// Print an error message and terminate the program
void sys_err(const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

// Change the hostname and print the old and new values
void ch_hostname(const char* new_hostname){
    char hostname[64];

    // Get the current hostname
    if(gethostname(hostname, sizeof(hostname))){
        sys_err("gethostname() failed");
    };

    printf("Hostname changing: %s ==> ", hostname);

    // Set the new hostname
    if(sethostname(new_hostname,sizeof(new_hostname))){
        sys_err("sethostname() failed");
    };

    // Get the updated hostname
    if(gethostname(hostname, sizeof(hostname))){
        sys_err("gethostname() failed");
    };

    printf("%s\n", hostname);
}

void run_container(){

    ch_hostname("new");

    // Start bash
    char *args[] = {"/bin/bash", NULL};
    if(execv("/bin/bash",args)){
        sys_err("execv() failed");
    }

}

void run(){

    if (unshare(CLONE_NEWUTS)){
        sys_err("unshare() failed");
    }

    printf("Namespace created successfully.\n");

    run_container();

}