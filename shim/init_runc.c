#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <pty.h>
#include "common.h"

#define BUFFER_SIZE 1024
#define container_process "/home/dadmin/docker/runc/bin/arunc"

int master_fd = 0;

void tty_config(int tty_fd){
    struct termios tty;
    if (tcgetattr(tty_fd, &tty) == -1) {
        log_err("Failed to get terminal attributes");
    }
    tty.c_lflag |= ICANON | ISIG;
    tty.c_lflag &= ~ECHO;  // disable echo
    if (tcsetattr(tty_fd, TCSANOW, &tty) == -1) {
        log_err("Failed to set terminal attributes");
    }
}

int init_runc(){
    if(master_fd){
        return master_fd;
    }

    int slave_fd;
    pid_t pid;
    char slave_name[BUFFER_SIZE];

    // create pseudo-terminal
    if(openpty(&master_fd, &slave_fd, slave_name, NULL, NULL) == -1){
        log_err("openpty");
    }

    pid = fork();
    if (pid == -1){
        log_err("fork");
    }

    // child process
    if (pid == 0){
        close(master_fd);
        tty_config(slave_fd);
        if (dup2(slave_fd, STDIN_FILENO) == -1 ||
            dup2(slave_fd, STDOUT_FILENO) == -1 ||
            dup2(slave_fd, STDERR_FILENO) == -1) {
            log_err("dup2");
        }

        close(slave_fd);

        execlp(container_process, "arunc", NULL);
        log_err("cannot open arunc");
    } else{
        // parent process
        close(slave_fd);
        return master_fd;
    }
}