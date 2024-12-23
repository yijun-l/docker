#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

struct custom_data{
    int fd;
    int forward_fd;
};

void run_event_loop();

#endif