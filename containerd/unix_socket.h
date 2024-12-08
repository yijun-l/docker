#ifndef UNIX_SOCKET_H
#define UNIX_SOCKET_H

extern int listen_unix_socket(const char* sockfile);
extern void accept_connection(int server_fd);
extern void connect_unix_socket(const char* sockfile);
extern void forward_unix_socket(const char* sockfile, char* msg);

#endif