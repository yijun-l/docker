#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"
#include "unix_socket.h"


#define SOCK_FILE "/var/run/ashim.sock"
#define MAX_EVENTS 10

int epoll_init(){
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1){
        log_err("epoll_create1() failed");
    }
    return epoll_fd;
}

int epoll_add_read_event(int epoll_fd, int event_fd){
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = event_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event) == -1){
        xlog("epoll_ctl() EPOLL_CTL_ADD failed");
        return -1;
    }
    return 0;
}

void run_event_loop(){
    struct epoll_event events[MAX_EVENTS];

    int server_fd = listen_unix_socket(SOCK_FILE);
    int epoll_fd = epoll_init();
    epoll_add_read_event(epoll_fd, server_fd);

    while(1){
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if(event_count == -1){
            log_err("epoll_wait() failed");
        }
        for (int i = 0; i < event_count; i++){
            if (events[i].data.fd == server_fd){
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1){
                    log_err("accept() failed");
                }
                xlog("Accepted new connection (fd: %d)", client_fd);
                epoll_add_read_event(epoll_fd, client_fd);
            } else {
                char buffer[128];
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = read(events[i].data.fd, buffer, sizeof(buffer) - 1);
                if (bytes_read == 0){
                    xlog("connection (fd: %d) disconnected", events[i].data.fd);
                    close(events[i].data.fd);
                }
                if (bytes_read > 0){
                    xlog("Received message from client %s", buffer);
                }

                const char* response = "Reply from ashim\n";
                write(events[i].data.fd, response, strlen(response));
            }
        }

        
    }
    xlog("event_loop completed.");
}