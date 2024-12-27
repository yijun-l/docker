#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// Print an error message and terminate the program
void sys_err(const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

// log module

static FILE* logfile = NULL;

void xlog_init(const char* filename){
    logfile = fopen(filename, "a");
    if(!logfile){
        sys_err("failed to open or create log file");
    }
}

void xlog(const char *format, ...) {
    if (logfile == NULL) {
        sys_err("Log file doesn't open");
    }

    char timestamp[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    fprintf(logfile, "[%s] %s[%d]: ", timestamp, "containerd", getpid());

    va_list args;
    va_start(args, format);
    vfprintf(logfile, format, args);
    va_end(args);

    fprintf(logfile, "\n");
    fflush(logfile); 
}

void xlog_close(){
    if (logfile == NULL){
        sys_err("Log file doesn't exist");
    }
    if (fclose(logfile)){
        sys_err("failed to close() log file");
    }
    logfile == NULL;
}

// write an error message to log file
void log_err(const char* msg){
    xlog("%s: %s", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

// daemonlize module

void write_pid(const char* pidfile){
    FILE* pid_file = fopen(pidfile, "w");
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

void go_background(const char* pidfile){

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

    write_pid(pidfile);

}