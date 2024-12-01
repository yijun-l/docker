# Control Groups

**Control Groups**, commonly referred to as **cgroups**, is a powerful Linux kernel feature that allows system administrators to allocate and manage resources such as CPU, memory, network bandwidth, and more among user-defined groups of processes. This dynamic resource management capability helps in monitoring, optimizing system performance, and ensuring fair resource allocation among different processes and users.

There are two versions of cgroups:

- **Cgroup v1** is the original implementation of cgroups in Linux. It uses multiple hierarchies, where each hierarchy manages a single resource, such as CPU, memory, or I/O. In v1, a resource can only be managed under one hierarchy at a time, and each hierarchy can have different subsystems attached to it. 

- **Cgroup v2** is the newer and more unified implementation of cgroups. Unlike v1, cgroup v2 uses a single hierarchy for all resources, making it simpler and more flexible to manage. In cgroup v2, all resources are treated uniformly, and tasks can be grouped in a single hierarchy, regardless of the resource type.

## Resource Organization

Cgroups have multiple, independent hierarchies that can coexist on the same system. Each hierarchy is mounted under the `/sys/fs/cgroup/` directory and this modularity allows for fine-grained resource control.

The following are the primary subsystems provided by cgroups:

- **blkio**: Sets limits on I/O access to block devices (e.g., disks, SSDs, USB drives).
- **cpu**: Allocates CPU time to tasks using the CPU scheduler.
- **cpuacct**: Generates automatic reports on CPU usage by tasks.
- **cpuset**: Assigns specific CPUs and memory nodes to tasks on a multicore system.
- **devices**: Controls access to devices for tasks (e.g., granting or denying access).
- **freezer**: Suspends or resumes tasks within a cgroup.
- **memory**: Limits memory usage by tasks and provides memory usage statistics.
- **net_cls**: Tags network packets with identifiers to manage traffic using Linux tc (traffic control).
- **net_prio**: Dynamically sets network traffic priority for different interfaces.
- **perf_event**: Tracks cgroup membership for performance analysis and monitoring.

## Testing

### Install stress test tool

```bash
$ sudo yum install epel-release -y
$ sudo yum install stress -y
```

### Stress testing on CPU
```bash
$ sudo mkdir /sys/fs/cgroup/cpu/test

# Without limit
$ stress --cpu 1
$ top
    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
  15022 dadmin    20   0    7976     96      0 R  93.8   0.0   0:28.43 stress

# Limit the CPU time to 50%
$ echo 50000 | sudo tee /sys/fs/cgroup/cpu/test/cpu.cfs_quota_us
50000

$ echo $$ | sudo tee /sys/fs/cgroup/cpu/test/tasks
11963

$ cat /proc/$$/cgroup
12:devices:/
11:memory:/
10:freezer:/
9:cpu,cpuacct:/test
8:rdma:/
7:cpuset:/
6:net_cls,net_prio:/
5:hugetlb:/
4:perf_event:/
3:pids:/user.slice/user-1000.slice/session-33.scope
2:blkio:/
1:name=systemd:/user.slice/user-1000.slice/session-33.scope

$ stress --cpu 1
$ top
    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
  15090 dadmin    20   0    7976     96      0 R  50.3   0.0   0:05.78 stress

```

### Stress testing on Memory

```bash
$ sudo mkdir /sys/fs/cgroup/memory/test

$ free -m
              total        used        free      shared  buff/cache   available
Mem:           1778         210        1425           3         142        1421
Swap:          1639         117        1522

# Without limit
$ stress --vm 1 --vm-bytes 200M
$ top
   PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
  15111 dadmin    20   0  212780 205036    212 R  50.0  11.3   0:02.22 stress

# Limit the Memory to 100M
$ echo 100M | sudo tee /sys/fs/cgroup/memory/test/memory.limit_in_bytes
100M

$ echo $$ | sudo tee /sys/fs/cgroup/cpu/test/tasks
11963

$ cat /proc/$$/cgroup
12:devices:/
11:memory:/test
10:freezer:/
9:cpu,cpuacct:/test
8:rdma:/
7:cpuset:/
6:net_cls,net_prio:/
5:hugetlb:/
4:perf_event:/
3:pids:/user.slice/user-1000.slice/session-33.scope
2:blkio:/
1:name=systemd:/user.slice/user-1000.slice/session-33.scope

$ stress --vm 1 --vm-bytes 200M
$ top
    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
  15196 dadmin    20   0  212780 101708    276 R  24.5   5.6   0:08.10 stress
```
<br>

# Cgroup Namespace

A **cgroup namespace** isolates how processes view the cgroup hierarchy. By creating a cgroup namespace, processes see only the subtree they belong to, and this subtree appears as the full hierarchy. This isolation ensures that processes inside a namespace cannot access or manage cgroups outside their view, which is particularly useful in containerized environments.

### Before Namespace Creation

The `bash` process belongs to the global cgroup hierarchy.

```bash
$ cat /proc/$$/cgroup
12:devices:/system.slice/sshd.service
11:memory:/user.slice/user-1000.slice/session-34.scope
10:freezer:/
9:cpu,cpuacct:/
8:rdma:/
7:cpuset:/
6:net_cls,net_prio:/
5:hugetlb:/
4:perf_event:/
3:pids:/user.slice/user-1000.slice/session-34.scope
2:blkio:/system.slice/sshd.service
1:name=systemd:/user.slice/user-1000.slice/session-34.scope
```

### After Namespace Creation

When a new cgroup namespace is created, the current cgroup subtree of the process becomes the root for the new namespace. Processes inside this namespace see paths relative to this root.

```bash
$ sudo unshare --cgroup

$ cat /proc/$$/cgroup
12:devices:/
11:memory:/
10:freezer:/
9:cpu,cpuacct:/
8:rdma:/
7:cpuset:/
6:net_cls,net_prio:/
5:hugetlb:/
4:perf_event:/
3:pids:/
2:blkio:/
1:name=systemd:/
```