# Namespace

Docker is written in the Go programming language and takes advantage of several features of the Linux kernel to deliver its functionality. One of the key technologies Docker relies on is namespaces, which provide the isolated workspace that we call a container. When you run a container, Docker creates a set of namespaces for that container.

Namespaces offer a layer of isolation by partitioning kernel resources such that each partition is unique to a specific namespace. This isolation ensures that processes running within one namespace are unaware of and unable to directly interact with processes or resources in another namespace. Each aspect of a container runs in its own namespace, with access strictly limited to that namespace.

Linux namespaces were introduced with the clone() system call in the Linux kernel, starting with the mount namespace in 2002. Over time, additional types of namespaces have been introduced, enabling isolation at various levels. Here are the most commonly used namespaces:

## Types of Namespaces

### Mount Namespace (CLONE_NEWNS)

- Isolates file system mount points.
- Containers can have their own file system hierarchy, ensuring that changes to mounted file systems in one container don’t affect others.

### UTS Namespace (CLONE_NEWUTS)

- Isolates hostname and domain name.
- Containers can have their own hostname, enabling them to appear as separate machines on a network.


### PID Namespace (CLONE_NEWPID)

- Isolates process IDs.
- Processes within a container have their own PID numbering, starting from 1 for the container's init process.

### Network Namespace (CLONE_NEWNET)

- Isolates networking resources, such as IP addresses, routing tables, and sockets.
- Containers can have their own virtual network interfaces.

### IPC Namespace (CLONE_NEWIPC)

- Isolates inter-process communication (e.g., shared memory, message queues, and semaphores).

### User Namespace (CLONE_NEWUSER)

- Isolates user and group IDs.
- Enables mapping container users to different user IDs on the host, enhancing security.

### Cgroup Namespace (CLONE_NEWCGROUP)

- Isolates cgroup root directories.
- Ensures that resource limits and accounting are specific to a container.

By leveraging these namespaces, Docker ensures that containers remain isolated and independent of the host system and other containers.

