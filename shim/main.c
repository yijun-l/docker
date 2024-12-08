#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"
#include "unix_socket.h"
#include "event_loop.h"

#define PID_FILE "/var/run/ashim.pid"
#define LOG_FILE "../log/adocker.log"
#define SOCK_FILE "/var/run/ashim.sock"


void handle_sigterm(int sig){
    xlog("Received SIGTERM (signal %d). Cleaning up and exiting...", sig);
    unlink(PID_FILE);
    exit(0);
}

int main(){
    xlog_init(LOG_FILE);

    go_background(PID_FILE);
    xlog("running as a daemon process");

    if(signal(SIGTERM, handle_sigterm) == SIG_ERR){
        log_err("signal() failed");
    }

    run_event_loop();
    

    xlog_close();
    return 0;
}