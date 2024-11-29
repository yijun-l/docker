#ifndef IPC_H
#define IPC_H

void create_ipc_components();
void remove_ipc_components();

// message queue
void msq_queue_check();
void msg_queue_open();
void msq_queue_close();

// shared memory
void shared_memory_open();
void shared_memory_close();

// semaphore
void semaphore_open();
void semaphore_close();



#endif