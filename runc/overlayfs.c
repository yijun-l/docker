#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include "common.h"
#include "overlayfs.h"


void setup_overlayfs(const char* lowerdir, const char* upperdir, const char* workdir, const char* mergedir){
    
    info("Setting up overlayfs\n");

    char mount_data[1024];
    memset(mount_data, 0, sizeof(mount_data));

    snprintf(mount_data, sizeof(mount_data), "lowerdir=%s,upperdir=%s,workdir=%s", lowerdir, upperdir, workdir);

    info("overlay info: %s\n", mount_data);

    if(mount("overlay", mergedir, "overlay", 0, mount_data) == -1){
            sys_err("mount() failed");
    }
}