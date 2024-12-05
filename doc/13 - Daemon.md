# Daemon

A **daemon** is a background service process that continuously runs to handle tasks or provide services to other applications or the system. 

Traditionally, daemons followed a set of steps derived from **SysV Unix**. However, with modern init systems like systemd, daemons have evolved into "new-style" daemons, which are simpler to implement and more powerful in functionality.

## SysV Daemons

**SysV daemons** require manual steps to set up and maintain their background process behavior.

### 1. Cleaning Up the Environment

Before running in the background, a daemon must ensure it operates independently of its parent process:

- **Close Extra File Descriptors**: Shut all unnecessary file descriptors, leaving only standard input, output, and error open. This avoids unintended access to resources.
- **Reset Signal Handlers**: Restore all signal handlers to their default state (`SIG_DFL`) to prevent interference from inherited configurations.
- **Unblock Signals**: Clear any blocked signals using `sigprocmask()` for clean operation.
- **Sanitize Environment Variables**: Reset or remove variables that might negatively affect the daemon's execution.

### 2. Detaching from the Terminal

To run as a true background process, a daemon detaches from the terminal:

- **First Fork**: The process forks itself, and the parent exits. This makes the child an orphan, reparented to the system supervisor (e.g., init or systemd), ensuring it remains active if the terminal session ends.
- **Create a New Session**: The child process calls `setsid()` to start a new session, breaking ties with the controlling terminal and becoming the session leader.
- **Second Fork**: A second fork ensures the process can never accidentally acquire a terminal. The parent of this fork exits, leaving a fully detached grandchild process.

### 3. Setting Up the Working Environment

After detachment, the daemon configures its environment for independent operation:

- **Redirect Standard I/O**: Redirect standard input, output, and error to `/dev/null` to avoid terminal interaction.
- **Set umask**: Reset the file creation mask to 0 for precise control over file permissions.
- **Change Working Directory**: Switch to the root directory (`/`) to prevent locking directories or blocking unmounts.

### 4. Maintaining Stability

To ensure smooth operation, a daemon handles additional housekeeping tasks:

- **Manage PID Files**: Write its process ID (PID) to a file (e.g., `/var/run/daemon-name.pid`) to prevent multiple instances from running simultaneously.
- **Drop Privileges**: If started as a privileged user (root), switch to a non-privileged user for security.
- **Notify Completion**: Use a mechanism (e.g., pipes) to inform the parent process or supervisor that initialization is complete and the daemon is ready.

### Example

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

void daemonlize(){
    // Parent process exits
    pid_t pid;
    if (pid = fork() > 0) {
            exit(EXIT_SUCCESS);
    }

    // Create a new session and detach from terminal
    pid_t sid = setsid();

    // Change the working directory to root to avoid locking mount points
    chdir("/");

    // Close all open file descriptors except stdin, stdout, stderr
    for (int fd = 3; fd < sysconf(_SC_OPEN_MAX); fd++) {
            close(fd);
    }

    // Redirect standard I/O to /dev/null
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "a", stdout);
    freopen("/dev/null", "a", stderr);
}

int main(){
    daemonlize();
    while(1){
        sleep(100);
    }
    
    return 0;
}
```

```bash
$ ./sysv_daemon

$ ps -o pid,ppid,pgid,sid,comm -e | grep sysv
    PID    PPID    PGID     SID COMMAND
   6857       1    6857    6857 sysv_daemon

$  ll /proc/6857/fd
total 0
lr-x------. 1 root root 64 Dec  5 11:50 0 -> /dev/null
l-wx------. 1 root root 64 Dec  5 11:50 1 -> /dev/null
l-wx------. 1 root root 64 Dec  5 11:50 2 -> /dev/null

$ pwdx 6857
6857: /
```


## New-Style Daemons

New-style daemons simplify implementation and improve compatibility with modern init systems like systemd. These daemons delegate most setup tasks to the init system, allowing developers to focus on core functionality. See key features below:

### 1. No Manual Setup

Modern init systems handle file descriptor management, signal resetting, session creation, and environment sanitization. Developers don’t need to implement these steps manually.

### 2. Systemd Integration

- **Standardized Logging**: Standard output and error are automatically captured and managed by `systemd-journald`. This allows daemons to log using simple `fprintf()` statements to `stderr`.
- **Service Notifications**: Daemons use `sd_notify()` to communicate status changes (e.g., `READY=1` after initialization or `STOPPING=1` before shutdown).
- **Clean Shutdown**: When receiving signals like `SIGTERM`, new-style daemons gracefully shut down and inform `systemd` of their state.

### 3. Resource Management

Instead of implementing custom resource limits or privilege dropping, new-style daemons rely on `systemd` for these controls. 

For example, developers can use `systemd` directives like `User=`, `LimitNOFILE=`, and `ReadOnlyPaths=` in the service file.

### 4. Simplified Deployment

A new-style daemon only needs to define its behavior and provide a `.service` file for configuration.


### Example

```c
// new_daemon.c
#include <unistd.h>

int main(){
    while(1) {
        sleep(100);
    }

    return 0;
}
```

```bash
$ gcc new_daemon.c -o /usr/local/bin/new_daemon

$ vim /usr/lib/systemd/system/new_daemon.service
[Service]
ExecStart=/usr/local/bin/new_daemon

[Install]
WantedBy=multi-user.target

$ systemctl enable new_daemon.service --now

$ systemctl status new_daemon.service
● new_daemon.service
   Loaded: loaded (/usr/lib/systemd/system/new_daemon.service; enabled; vendor >
   Active: active (running) since Thu 2024-12-05 14:29:28 CST; 20min ago
 Main PID: 7459 (new_daemon)
    Tasks: 1 (limit: 11127)
   Memory: 136.0K
   CGroup: /system.slice/new_daemon.service
           └─7459 /usr/local/bin/new_daemon

$ ps -o pid,ppid,pgid,sid,comm -p 7459
    PID    PPID    PGID     SID COMMAND
   7459       1    7459    7459 new_daemon

$  ll /proc/7459/fd
total 0
lr-x------. 1 root root 64 Dec  5 14:29 0 -> /dev/null
lrwx------. 1 root root 64 Dec  5 14:29 1 -> 'socket:[700513]'
lrwx------. 1 root root 64 Dec  5 14:29 2 -> 'socket:[700513]'

$   7459
7459: /
```