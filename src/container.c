#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include "../include/common.h"
#include "../include/overlayfs.h"

void run_container(){

    // Change hostname
    ch_hostname("new");

    // mapping user to /proc/$$/uid_map
    setup_user_namespace();

    pid_t pid = fork();

    if (pid == -1){
        sys_err("fork() failed");
    }

    if (pid == 0){
        printf("In PID Namespace, current pid changed to: %d\n", getpid());

        if(chroot("rootfs/merged_dir") == -1){
            sys_err("chroot() failed");
        }

        if(chdir("/") == -1){
            sys_err("chdir() failed");
        }

        // Mount procfs
        if(mount("proc", "/proc", "proc", 0, NULL) != 0){
            sys_err("mount() failed");
        }
        
        // Start bash
        char *args[] = {"/bin/bash", NULL};
        if(execv("/bin/bash",args)){
            sys_err("execv() failed");
        }
    } else {
        wait(NULL);
    }
}

void run(){

    setup_overlayfs("rootfs/lower_dir", "rootfs/upper_dir", "rootfs/work_dir", "rootfs/merged_dir");

    // create user/uts namespace
    if (unshare(CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWPID | CLONE_NEWNS)){
        sys_err("unshare() failed");
    }

    printf("Namespace created successfully.\n");

    run_container();

}