#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "system.h"
#include "common.h"

#define VER "1.0"
#define DATE "12/26/2024"
#define SOCK_FILE "/var/run/acontainerd.sock"
#define BUFFER_SIZE 256

char* program_name = "adocker";

void usage (int status){
    if (status != EXIT_SUCCESS){
        emit_try_help();
    }
    else{
        printf("\nUsage: %s [OPTION]...\n", program_name);
        fputs("\
\n\
    adocker is a client for interacting with the daemon through the CLI.\n\
\n\
        help        display this help and exit\n\
        version     output version information and exit\n\
        connect     connect to a bash container\n\
\n\
", stdout);
    }

    exit (status);
}

int contains_pattern(const char *str) {
    const char *prefix = "[";
    const char *middle = "@";
    const char *suffix = "]";

    char *start = strstr(str, prefix);
    if (start) {
        char *at_sign = strstr(start + 1, middle);
        if (at_sign) {
            char *end = strstr(at_sign + 1, suffix);
            if (end) {
                return 1; 
            }
        }
    }
    return 0; 
}

int connect_sock_file(){
    struct sockaddr_un server_addr;
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1){
        sys_err("socket() failed");
    }
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_FILE, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1){
        sys_err("connect() failed");
    }
    return client_fd;
}

void show_version(){
    printf("version %s, %s\n", VER, DATE);
}

void docker_ps(){
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int client_fd = connect_sock_file();
    const char* msg = "ps";
    if(send(client_fd, msg, strlen(msg), 0) == -1){
        sys_err("send()");
    }
    int received_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if(received_bytes == -1){
        sys_err("recv() failed");
    }
    printf("==> %s\n", buffer);
    close(client_fd);
}

void docker_connect(){
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int client_fd = connect_sock_file();
    const char* msg = "connect";
    if(send(client_fd, msg, strlen(msg), 0) == -1){
        sys_err("send()");
    }
    int received_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if(received_bytes == -1){
        sys_err("recv() failed");
    }
    if(received_bytes == 0){
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("%s", buffer);
    
    while(1){
        memset(buffer, 0, sizeof(buffer));
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            sys_err("fgets()");
        }
        if (strcmp(buffer, "exit\n") == 0) {
            // TODO: leave the session, close fd.
            printf("Exiting client.\n");
            exit(EXIT_SUCCESS);
        }

        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            sys_err("send()");
        }
       
       do{
        memset(buffer, 0, sizeof(buffer));
        received_bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        printf("%s", buffer);
        } while (!contains_pattern(buffer));
    }
    
}

int main(int argc, char* argv[]){

    if (argc != 2){
        usage(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "help") == 0){
        usage(EXIT_SUCCESS);
    } else if (strcmp(argv[1], "version") == 0){
        show_version();
    } else if (strcmp(argv[1], "ps") == 0){
        docker_ps();
    } else if (strcmp(argv[1], "start") == 0){
        show_version();
    } else if (strcmp(argv[1], "stop") == 0){
        show_version();
    } else if (strcmp(argv[1], "connect") == 0){
        docker_connect();
    }else {
        usage(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}