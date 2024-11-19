#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <fcntl.h>


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

void write_file(const char* filename, const char* content){
    int fd = open(filename, O_WRONLY);

    if (fd == -1){
        sys_err("open() failed");
    }

    int ret = write(fd, content, strlen(content));
    if ( ret == -1){
        sys_err("write() failed");
    }

    close(fd);
}

void setup_user_namespace(){
    char buf[64];
    pid_t current_pid = getpid();
    printf("before, UID: %d\n", getuid());

    sprintf(buf, "/proc/%d/setgroups", current_pid);
    write_file(buf, "deny\n");

    sprintf(buf, "/proc/%d/uid_map", current_pid);
    write_file(buf, "0 0 1\n");

    sprintf(buf, "/proc/%d/gid_map", current_pid);
    write_file(buf, "0 0 1\n");

     printf("before, UID: %d\n", getuid());
}

void run_container(){

    // Change hostname
    ch_hostname("new");

    // mapping user to /proc/$$/uid_map
    setup_user_namespace();

    // Start bash
    char *args[] = {"/bin/bash", NULL};
    if(execv("/bin/bash",args)){
        sys_err("execv() failed");
    }


}

void run(){

    // create user/uts namespace
    if (unshare(CLONE_NEWUTS | CLONE_NEWUSER)){
        sys_err("unshare() failed");
    }

    printf("Namespace created successfully.\n");

    run_container();

}