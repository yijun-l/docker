#include <unistd.h>
#include "common.h"

#define PID_FILE "/var/run/ashim.pid"
#define LOG_FILE "../log/adocker.log"

int main(){
    xlog_init(LOG_FILE);

    go_background(PID_FILE);
    xlog("running as a daemon process");

    
    while(1){
        sleep(10);
    }

    xlog_close();
    return 0;
}