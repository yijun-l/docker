#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"
#include "unix_socket.h"
#include "event_loop.h"


#define SOCK_FILE "/var/run/acontainerd.sock"
#define SHIM_FILE "/var/run/ashim.sock"
#define MAX_EVENTS 10

int is_running = 0;
int is_connected = 0;
int container_fd = -1;
int counter = 0;

int epoll_init(){
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1){
        log_err("epoll_create1() failed");
    }
    return epoll_fd;
}

int epoll_add_read_event(int epoll_fd, int event_fd, int forward_fd){
    struct epoll_event event;
    struct custom_data* data = (struct custom_data*)malloc(sizeof(struct custom_data));
    event.events = EPOLLIN;
    event.data.ptr = (void*)data;
    data->fd = event_fd;
    data->forward_fd = forward_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event) == -1){
        log_err("epoll_ctl() EPOLL_CTL_ADD failed");
        return -1;
    }
    xlog("add %d to epoll", event_fd);
    return 0;
}

int epoll_remove_read_event(int epoll_fd, int event_fd){
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, NULL) == -1){
        log_err("epoll_ctl() EPOLL_CTL_DEL failed");
        return -1;
    }
    xlog("remove %d from epoll", event_fd);
    return 0;
}

void run_event_loop(){
    struct epoll_event events[MAX_EVENTS];

    int server_fd = listen_unix_socket(SOCK_FILE);
    int epoll_fd = epoll_init();
    epoll_add_read_event(epoll_fd, server_fd, -1);

    while(1){
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if(event_count == -1){
            log_err("epoll_wait() failed");
        }
        for (int i = 0; i < event_count; i++){
            struct custom_data* data = (struct custom_data*)events[i].data.ptr;
            if (data->fd == server_fd){
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1){
                    log_err("accept() failed");
                }
                xlog("Accepted new connection (fd: %d)", client_fd);
                epoll_add_read_event(epoll_fd, client_fd, -1);
            } else {
                char buffer[128];
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = read(data->fd, buffer, sizeof(buffer) - 1);
                if (bytes_read == 0){
                    xlog("connection (fd: %d) disconnected", data->fd);
                    is_connected = 0;
                    epoll_remove_read_event(epoll_fd, data->fd);
                    if(data->forward_fd != -1){
                        epoll_remove_read_event(epoll_fd, data->forward_fd);
                    }
                    close(data->fd);
                    free(data);
                }
                
                if (bytes_read > 0){
                    if (is_connected){
                        write(data->forward_fd, buffer, strlen(buffer));
                    
                    } else if (strcmp(buffer, "ps") == 0){
                        memset(buffer, 0, sizeof(buffer));
                        if(is_running){
                            strcpy(buffer, "Bash container is running...");
                        } else {
                            strcpy(buffer, "Bash container is NOT running...");
                        }
                        write(data->fd, buffer, strlen(buffer));
                    } else if (strcmp(buffer, "start") == 0){
                        memset(buffer, 0, sizeof(buffer));
                        if (is_running){
                            strcpy(buffer, "Bash container already running...");
                            xlog("<docker start> container already started. container fd: %d, is_running: %d", container_fd, is_running);
                        } else {
                            strcpy(buffer, "Bash container is running now ...");
                            container_fd = get_forward_unix_socket_fd(SHIM_FILE);
                            // data->forward_fd = container_fd;
                            is_running = 1;
                            xlog("<docker start> container started. container fd: %d, is_running: %d", container_fd, is_running);
                        }
                        write(data->fd, buffer, strlen(buffer));

                    } else if (strcmp(buffer, "attach") == 0 && !is_connected){
                        memset(buffer, 0, sizeof(buffer));
                        if (!is_running){
                            strcpy(buffer, "==> Bash container is NOT running...");
                        } else {
                            counter++;
                            data->forward_fd = container_fd;
                            xlog("data->forward_fd = %i, data->fd = %i", data->forward_fd, data->fd);
                            
                            epoll_add_read_event(epoll_fd, data->forward_fd, data->fd);
                            if(counter == 1){
                                write(data->forward_fd, " ", 1); 
                            } else {
                                write(data->forward_fd, "\n", 1); 
                            }                                     
                            bytes_read = read(data->forward_fd, buffer, sizeof(buffer) - 1);
                            xlog("received from bash: %s", buffer);
                            if(buffer[0] == ' '){
                                memmove(buffer, buffer + 1, strlen(buffer));
                            }
                            is_connected = 1;
                        }
                        xlog("<docker connect> buffer: %s. container fd: %d, is_running: %d", buffer, container_fd, is_running);
                        write(data->fd, buffer, strlen(buffer));
                    }
                }
            }
        }

        
    }
    xlog("event_loop completed.");
}