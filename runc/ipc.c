#include "common.h"
#include <stdio.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MQ_NAME "/av_mq"
#define SHM_NAME "/av_shm"
#define SEM_NAME "/av_sem"

void msg_queue_open(){
    if (mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, NULL) == (mqd_t)-1){
        sys_err("mq_open() failed");
    }
}

void msq_queue_check(){
    mqd_t mq = mq_open(MQ_NAME, O_RDWR);
    if (mq == (mqd_t)-1) {
        sys_err("mq_open() failed");
    }

    struct mq_attr attr;
    if (mq_getattr(mq, &attr) == -1) {
        sys_err("mq_getattr() failed");
    }

    printf("Queue attributes:\n");
    printf("  Max messages: %ld\n", attr.mq_maxmsg);
    printf("  Message size: %ld\n", attr.mq_msgsize);
    printf("  Current messages: %ld\n", attr.mq_curmsgs);
}

void msq_queue_close(){
    if (mq_unlink(MQ_NAME) == -1){
        sys_err("mq_unlink() failed");
    }
}

void shared_memory_open(){
    if (shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666) == -1){
        sys_err("shm_open() failed");
    }

}

void shared_memory_close(){
    if (shm_unlink(SHM_NAME) == -1){
        sys_err("shm_unlink() failed");
    }
}

void semaphore_open(){
    if (sem_open(SEM_NAME, O_CREAT) == SEM_FAILED){
        sys_err("sem_open() failed");
    }
}

void semaphore_close(){
    if (sem_unlink(SEM_NAME) == -1){
        sys_err("sem_unlink() failed");
    }
}

void create_ipc_components(){
    msg_queue_open();
    shared_memory_open();
    semaphore_open();
    info("IPC resource (message queue, shared memory and semaphores) created.\n");
}

void remove_ipc_components(){
    msq_queue_close();
    shared_memory_close();
    semaphore_close();
    info("IPC resource (message queue, shared memory and semaphores) deleted.\n");
}