# Directories
LOG_DIR = log

# Start daemon processes
all:
	$(MAKE) -C containerd run
	$(MAKE) -C shim run

# Clean up the build, bin and rootfs directories
clean:
	rm -rf $(LOG_DIR)
	$(MAKE) -C containerd clean
	$(MAKE) -C shim clean
	$(MAKE) -C runc clean

# containerd
cd:
	$(MAKE) -C containerd run

ccd:
	$(MAKE) -C containerd clean

# shim
sm:
	$(MAKE) -C shim run

csm:
	$(MAKE) -C shim clean

# runc
rc:
	$(MAKE) -C runc run

crc:
	$(MAKE) -C runc clean