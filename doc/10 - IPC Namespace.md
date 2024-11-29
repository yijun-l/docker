# IPC

**Inter-process communication (IPC)** refers to the mechanisms that allow processes to communicate and synchronize their actions. In a multi-process environment, each process operates in its own memory space, and IPC mechanisms provide a way for them to exchange data, synchronize tasks, and cooperate in a controlled manner. IPC is crucial for developing complex applications that require interaction between multiple processes or even different machines.

The Linux kernel provides several IPC mechanisms, each designed for specific use cases. These mechanisms can be broadly classified into the following categories:

### 1. Pipes and FIFOs

- **Pipes** provide a unidirectional communication channel between processes. A pipe is a simple, low-overhead method of transferring data between related processes (typically parent-child).

- **FIFOs (named pipes)** extend the pipe concept by allowing communication between unrelated processes. Unlike regular pipes, FIFOs are identified by a filename and persist beyond the life of the processes that created them.

### 2. Message Queues

**Message queues** allow processes to communicate by sending and receiving messages in a structured manner. A message queue stores messages until they are retrieved by another process. This mechanism supports asynchronous communication, where a sender and receiver do not need to be synchronized in real-time.

### 3. Semaphores

**Semaphores** are synchronization primitives used to control access to shared resources. They help prevent race conditions by ensuring that only one process can access a critical section at a time. Semaphores can be used for mutual exclusion (mutexes) or for managing a set of resources with counting semaphores.

### 4. Shared Memory

**Shared memory** is the fastest IPC mechanism available in Linux. It allows multiple processes to access a common memory region directly. Since no data copying is required, shared memory offers high performance and is ideal for applications that require fast data exchange between processes.

### 5. Sockets

**Sockets** provide a method for inter-process communication, often used in network communication. A socket can be used for communication between processes on the same machine (using Unix domain sockets) or over a network (using Internet sockets).

### 6. Signals

**Signals** are a form of limited inter-process communication that allow processes to send simple notifications to one another. Signals are primarily used for event handling, such as notifying a process about a particular event.

<br>

# IPC Namespace

The IPC namespace is a feature of the Linux kernel that isolates IPC resources between different processes. Each process can be placed in an IPC namespace, which gives it access to its own set of IPC resources, such as **message queues**, **semaphores**, and **shared memory**. Processes in different IPC namespaces cannot directly access each other's IPC resources, providing an additional layer of security and isolation.

IPC namespaces are particularly useful in containerized environments, like Docker, where containers need to be isolated from each other and the host system. By using IPC namespaces, containers can have their own IPC mechanisms, preventing them from interacting with or affecting processes running outside their namespace.

## Testing

### Create IPC Resources

```bash
$ ipcmk -Q
Message queue id: 0

$ ipcmk -M 1024
Shared memory id: 0

$ ipcmk -S 10
Semaphore id: 3

$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x8636e09d 0          root       644        0            0

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x71f99284 0          root       644        1024       0

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0xebc47387 3          root       644        10
```

### Check IPC Resources Without IPC Namespace

```bash
$ unshare --fork

$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x8636e09d 0          root       644        0            0

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x71f99284 0          root       644        1024       0

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0xebc47387 3          root       644        10
```

### Check IPC Resources With IPC Namespace

```bash
$ sudo unshare --fork --ipc

$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
```

### Remove IPC Resources

```bash
$ ipcrm -q 0

$ ipcrm -m 0

$ ipcrm -s 3

$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
```