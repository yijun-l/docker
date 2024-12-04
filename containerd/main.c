#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

#define PID_FILE "/var/run/acontainerd.pid"
#define LOG_FILE "../log/adocker.log"

void write_pid(){
    FILE* pid_file = fopen(PID_FILE, "w");
    if(!pid_file){
        sys_err("fopen() failed");
    }
    fprintf(pid_file, "%d\n", getpid());
    fclose(pid_file);
}

void xclose(int fd){
    if(close(fd) == -1){
        sys_err("close() failed");
    }
}

int xopen(int fd, const char* file_name, int flags){
    int rfd;

    rfd = open(file_name, flags);
    if (rfd == -1){
        sys_err("open() failed");
    } else if(fd >=0 && rfd != fd){
        sys_err("open() succeed, but fd is incorrect");
    }
    return rfd;
}

static void go_background(){

    xclose(STDIN_FILENO);
    xopen(STDIN_FILENO, "/dev/null", O_RDONLY);

    xclose(STDOUT_FILENO);
    xopen(STDOUT_FILENO, "/dev/null", O_WRONLY);

    xclose(STDERR_FILENO);
    xopen(STDERR_FILENO, "/dev/null", O_WRONLY);

    pid_t pid = fork();

    if (pid == -1){
        sys_err("fork() failed");
    }

    if (pid != 0){
        /* parent process */
        exit(EXIT_SUCCESS);
    }
    
    /* child process */

    if (setsid() == (pid_t)-1){
        sys_err("setsid() failed");
    }

    if (chdir("/") == -1){
        sys_err("chdir() failed");
    }
   
    umask(0);

    write_pid();

}

int main(){
    xlog_init(LOG_FILE);

    go_background();
    xlog("running as a daemon process");

    
    while(1){
        sleep(10);
    }

    xlog_close();
    return 0;
}