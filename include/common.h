#ifndef COMMON_H
#define COMMON_H

// Print an error message and terminate the program
extern void sys_err(const char* msg);

// Change the hostname and print the old and new values
extern void ch_hostname(const char* new_hostname);

// Map uid/gid
extern void setup_user_namespace();

#endif