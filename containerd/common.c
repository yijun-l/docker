#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

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

