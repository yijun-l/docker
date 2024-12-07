#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "common.h"

#define BUFFER_SIZE 128

int listen_unix_socket(const char* sockfile){
    struct sockaddr_un server_addr;

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1){
        log_err("socket() failed");
    }

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, sockfile, sizeof(server_addr.sun_path) - 1);

    unlink(sockfile);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        close(server_fd);
        log_err("bind() failed");
    }

    if(listen(server_fd, 5) == -1){
        close(server_fd);
        log_err("listen() failed");
    }

    xlog("server is waiting for connections...");

    return server_fd;
}

void accept_connection(int server_fd){
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1){
        log_err("accept() failed");
    }
    
    int buffer[BUFFER_SIZE];

    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0){
        xlog("Received message from client %s", buffer);
    }

    const char* response = "yes!";
    write(client_fd, response, strlen(response));
    close(client_fd);
    close(server_fd);
}

void connect_unix_socket(const char* sockfile){
    struct sockaddr_un server_addr;

    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1){
        log_err("socket() failed");
    }

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, sockfile, sizeof(server_addr.sun_path) - 1);

    if(connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        close(client_fd);
        log_err("connect() failed");
    }

    int buffer[BUFFER_SIZE];

    const char *message = "Hello!";
    write(client_fd, message, strlen(message));

    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0){
        xlog("Received message from server %s", buffer);
    }

    close(client_fd);
}