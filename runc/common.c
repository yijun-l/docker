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
#include "common.h"

#define APP "runc"

// Print an error message and terminate the program
void sys_err(const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

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

    fprintf(logfile, "[%s] %s[%d]: ", timestamp, APP, getpid());

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
    logfile = NULL;
}

// write an error message to log file
void log_err(const char* msg){
    xlog("%s: %s", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

// Change the hostname and print the old and new values
void ch_hostname(const char* new_hostname){
    char hostname[64];

    // Get the current hostname
    if(gethostname(hostname, sizeof(hostname))){
        log_err("gethostname() failed");
    };

    xlog("Hostname changing: %s ==> ", hostname);

    // Set the new hostname
    if(sethostname(new_hostname,sizeof(new_hostname))){
        log_err("sethostname() failed");
    };

    // Get the updated hostname
    if(gethostname(hostname, sizeof(hostname))){
        log_err("gethostname() failed");
    };
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
    xlog("before => UID: %d\n", getuid());

    sprintf(buf, "/proc/%d/setgroups", current_pid);
    write_file(buf, "deny\n");

    sprintf(buf, "/proc/%d/uid_map", current_pid);
    write_file(buf, "0 0 1\n");

    sprintf(buf, "/proc/%d/gid_map", current_pid);
    write_file(buf, "0 0 1\n");

    xlog("after  => UID: %d\n", getuid());
}