#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include "common.h"
#include "unix_socket.h"
#include "event_loop.h"

#define PID_FILE "/var/run/ashim.pid"
#define LOG_FILE "../log/adocker.log"
#define SOCK_FILE "/var/run/ashim.sock"


void handle_sigterm(int sig){
    xlog("Received SIGTERM (signal %d). Cleaning up and exiting...", sig);
    unlink(PID_FILE);
    unlink(SOCK_FILE);
    exit(0);
}

void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        xlog("Child process %d exited with status %d\n", pid, status);
    }
}

int main(){
    xlog_init(LOG_FILE);

    go_background(PID_FILE);
    xlog("running as a daemon process");

    if(signal(SIGTERM, handle_sigterm) == SIG_ERR || signal(SIGCHLD, handle_sigchld) == SIG_ERR){
        log_err("signal() failed");
    }
    if (prctl(PR_SET_CHILD_SUBREAPER, 1) == -1){
        log_err("Failed to set subreaper");
    }

    run_event_loop();
    

    xlog_close();
    return 0;
}