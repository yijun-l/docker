#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include "common.h"
#include "overlayfs.h"
#include "ipc.h"

void run_container(){

    // Change hostname
    ch_hostname("new");

    // mapping user to /proc/$$/uid_map
    setup_user_namespace();

    pid_t pid = fork();

    if (pid == -1){
        log_err("fork() failed");
    }

    if (pid == 0){
        xlog("In PID Namespace, current pid changed to: %d\n", getpid());

        // char cwd1[1024];
        // if (getcwd(cwd1, sizeof(cwd1)) == NULL) {
        //     log_err("getcwd");
        // }
        // xlog("current dir: %s", cwd1);


        // if(chroot("/home/dadmin/docker/runc/rootfs/merged_dir") == -1){
        if(chroot("rootfs/merged_dir") == -1){
            log_err("chroot() failed");
        }

        if(chdir("/") == -1){
            log_err("chdir() failed");
        }

        // char cwd[1024];
        // if (getcwd(cwd, sizeof(cwd)) == NULL) {
        //     log_err("getcwd");
        // }
        // xlog("current dir: %s", cwd);

        // Mount procfs
        if(mount("proc", "/proc", "proc", 0, NULL) != 0){
            log_err("mount() proc failed");
        }

        // Start bash
        char *args[] = {"/bin/bash", NULL};
        if(execv("/bin/bash",args)){
            log_err("execv() failed");
        }
    } 
    else {
        wait(NULL);
        
    }
}

void run(){

    xlog("runc initialing...");

    if(chdir("/home/dadmin/docker/runc") == -1){
        log_err("chdir() to /home/dadmin/docker/runc failed");
    }

    // setup_overlayfs("/home/dadmin/docker/runc/rootfs/lower_dir", "/home/dadmin/docker/runc/rootfs/upper_dir", "/home/dadmin/docker/runc/rootfs/work_dir", "/home/dadmin/docker/runc/rootfs/merged_dir");
    setup_overlayfs("rootfs/lower_dir", "rootfs/upper_dir", "rootfs/work_dir", "rootfs/merged_dir");

    create_ipc_components();

    // create user/uts namespace
    if (unshare(CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWCGROUP)){
        log_err("unshare() failed");
    }

    info("Namespace created successfully.\n");

    // used for IPC NS testing
    // msq_queue_check();

    run_container();
}