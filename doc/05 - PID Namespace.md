# procfs 

The **procfs (process file system)** is a virtual file system in Linux, mounted at `/proc`. It provides a live, file-based interface to kernel information. By interacting with the files and directories within `/proc`, users can monitor system performance, gather details about running processes, and even configure kernel parameters at runtime.

### Key Features of procfs

- **Live System Information**: Procfs provides a real-time snapshot of the operating system's resources and processes. For instance, files like `/proc/meminfo` and `/proc/cpuinfo` show the current state of memory usage and CPU details.

- **Dynamic Updates**: The data within procfs files is constantly updated by the kernel, ensuring that users always see the most current information.

- **Kernel Configuration Interface**: Certain files in `/proc` enable users to adjust system behavior without restarting. For example, files under `/proc/sys/` allow runtime modifications to networking or process limits.

Procfs is a critical resource for system administrators and tools like `top`, `ps`, and `sysctl`, providing an efficient way to access and manage system internals.

## The /proc Directory

The `/proc` directory is the root of the procfs file system, housing files and directories that expose kernel data. Here’s an overview of its structure and significance.

### 1. System Information Files

Files in /proc provide a wealth of information about the system’s hardware and performance. Some key examples include:

- `/proc/cpuinfo`: Displays CPU details, such as model, speed, and core count.
- `/proc/meminfo`: Shows memory usage statistics, including free, available, and cached memory.
- `/proc/version`: Provides kernel version and build information.

These files are essential for monitoring system health and diagnosing performance issues.

### 2. Process-Specific Directories

Each running process on the system has a corresponding directory under `/proc`, named after its process ID (PID). These directories contain detailed information about the process, such as:

- `/proc/[pid]/cmdline`: Command-line arguments used to start the process.
- `/proc/[pid]/status`: Metadata, including the process’s state, user ID, and memory usage.
- `/proc/[pid]/fd/`: Links to the process’s open file descriptors.

For convenience, the symbolic link `/proc/self` always points to the directory of the process accessing `/proc`.

### 3. Kernel Configuration Files

The `/proc/sys/` directory houses tunable kernel parameters that control various aspects of the system:

- **Networking**: Files like `/proc/sys/net/ipv4/ip_forward` enable or disable IP forwarding.
- **Process Limits**: Parameters such as `/proc/sys/kernel/pid_max` set the maximum number of process IDs.
- **General Behavior**: Adjustments to settings like kernel panic behavior can be made here.

These files can be modified using commands like `sysctl` or by directly echoing values into them.

# PID Namespace

In Linux, every process is assigned a unique numeric identifier known as the Process ID (PID). These IDs are typically global across the system. However, in scenarios such as containers, where multiple environments need to coexist, sharing a global PID space can lead to conflicts or unintended interactions.

The PID namespace addresses this issue by providing isolated PID spaces for processes. Below are its key characteristics:

- **Isolation**: Processes in a PID namespace can only see and interact with other processes in the same namespace or its children. This ensures that processes in one namespace are invisible to processes in other namespaces, enhancing isolation.

- **Nested Namespaces**: PID namespaces can be nested, meaning a namespace can create child namespaces. A process in a parent namespace can see all processes in its child namespaces, but the reverse is not true.

- **Unique PID Hierarchy**: Within a PID namespace, the first process started (PID 1) acts as the "init" process of that namespace. It is responsible for reaping orphaned processes and managing signals within the namespace. If this process exits, the kernel terminates all other processes in the namespace.

### Testing via unshare

```bash
$ unshare --pid --fork

$ echo $$
1

$ mkdir proc
$ mount -t proc proc ./proc

$ ls proc
1          devices      ioports      loadavg       partitions   sysrq-trigger
33         diskstats    irq          locks         sched_debug  sysvipc
acpi       dma          kallsyms     mdstat        schedstat    thread-self
buddyinfo  driver       kcore        meminfo       scsi         timer_list
bus        execdomains  keys         misc          self         tty
cgroups    fb           key-users    modules       slabinfo     uptime
cmdline    filesystems  kmsg         mounts        softirqs     version
consoles   fs           kpagecgroup  mtrr          stat         vmallocinfo
cpuinfo    interrupts   kpagecount   net           swaps        vmstat
crypto     iomem        kpageflags   pagetypeinfo  sys          zoneinfo
```