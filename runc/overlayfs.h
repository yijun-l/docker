#ifndef OVERLAYFS_H
#define OVERLAYFS_H

extern void setup_overlayfs(const char* lowerdir, const char* upperdir, const char* workdir, const char* mergedir);

#endif