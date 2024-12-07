#ifndef COMMON_H
#define COMMON_H

// Print an error message and terminate the program
extern void sys_err(const char* msg);
extern void log_err(const char* msg);

// Log module
extern void xlog_init(const char* filename);
extern void xlog(const char *format, ...) ;
extern void xlog_close();
extern void go_background(const char* pidfile);

#endif