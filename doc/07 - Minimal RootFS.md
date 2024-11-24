# Building a Minimal Root Filesystem

After creating a namespace (with UTS, User, PID, and Mount), processes run in an isolated environment but still share the parent namespace's directory structure. This can lead to security risks, as processes might access or modify sensitive files, and operational conflicts between namespaces.

To achieve full directory isolation, we can assign a new root directory using the `chroot()` system call. However, the new root must include a minimal runtime environment (essential binaries, shared libraries, and configuration files) without which processes may fail.

Before constructing such an environment, let’s first understand what happens when a command is executed in the **Command-Line Interface (CLI)**.

## Command Parsing and Execution

When a command is executed in the shell, the process follows these steps:

### 1. Parsing the Command

The shell reads the user input and splits it into components: the command, its options, and arguments. It performs various expansions, such as resolving environment variables, path wildcards, and aliases. Finally, the shell searches for the command's executable file in the directories listed in the `$PATH` environment variable.

### 2. Creating a New Process

To execute the command, the shell calls `fork()` to create a child process. This child process is an exact copy of the shell, inheriting its environment variables and open file descriptors.

### 3. Executing the Command

In the child process, the `exec()` system call replaces the shell’s process image with the program specified by the command. 

The program is loaded into memory and prepared for execution.

### 4. Loading Shared Libraries

If the command is dynamically linked (as most modern Linux binaries are), the program loader invokes the dynamic linker (e.g., `/lib64/ld-linux-x86-64.so.2`). 

The linker identifies and maps the required shared libraries into memory, resolves symbol references, and ensures the program has access to its dependencies.

### 5. Starting Program Execution

Once the program is ready, control transfers to its entry point (usually the `main()` function). The program starts running, and the parent shell waits for it to finish (unless the command is executed in the background). 

After the program exits, the shell resumes control to accept new input.


## Dynamic Linker and Dependency Resolution

Modern Linux executables often rely on shared libraries, which are loaded dynamically at runtime. This modular design reduces memory usage and simplifies updates but also introduces a dependency on the dynamic linker.

To inspect the shared library dependencies of an executable, we use the `ldd` command. For example:

```bash
$ ldd `which bash`
        linux-vdso.so.1 (0x00007ffd3a3d3000)
        libtinfo.so.6 => /lib64/libtinfo.so.6 (0x00007f0db27b1000)
        libdl.so.2 => /lib64/libdl.so.2 (0x00007f0db25ad000)
        libc.so.6 => /lib64/libc.so.6 (0x00007f0db21e8000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f0db2cfc000)
```

Here’s what this output indicates:
- The **library name** (e.g., libc.so.6).
- The **path** to the library (e.g., /lib64/libc.so.6).
- The **memory address** where the library will be loaded at runtime (e.g., 0x00007f0db21e8000).

By understanding these dependencies, we can identify the libraries needed to create a functioning runtime environment.


## Minimal Root Filesystem

For processes in an isolated namespace to function correctly, the root directory must contain essential components, including:

- Essential binaries (`/bin` and `/sbin`).
- Shared libraries (`/lib` or `/lib64`) required by those binaries.

In our project, the minimal root filesystem is structured as follows:

```bash
rootfs
├── bin
│   ├── bash
│   ├── ls
│   └── ps
├── lib64
│   ├── ld-linux-x86-64.so.2
│   ├── libblkid.so.1
│   ├── libcap.so.2
│   ├── libc.so.6
│   ├── libdl.so.2
│   ├── libgcc_s.so.1
│   ├── libgcrypt.so.20
│   ├── libgpg-error.so.0
│   ├── liblz4.so.1
│   ├── liblzma.so.5
│   ├── libmount.so.1
│   ├── libpcre2-8.so.0
│   ├── libprocps.so.7
│   ├── libpthread.so.0
│   ├── librt.so.1
│   ├── libselinux.so.1
│   ├── libsystemd.so.0
│   ├── libtinfo.so.6
│   └── libuuid.so.1
```

Note, the script `copy_exe.sh` used to copy request command and corresponding library file.

After that, we can run our process in the separate namespace with the minimal root filesystem.

```bash
bash-4.4# ps -ef
UID          PID    PPID  C STIME TTY          TIME CMD
0              1       0  0 09:45 ?        00:00:00 /bin/bash
0              2       1  0 09:46 ?        00:00:00 ps -ef
bash-4.4# ls /
bin  lib64  proc
```
