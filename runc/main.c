#include "container.h"
#include "common.h"

#define LOG_FILE "/home/dadmin/docker/log/adocker.log"

int main(){
    xlog_init(LOG_FILE);
    run();
    xlog_close();
    return 0;
}