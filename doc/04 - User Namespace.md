# UID and GID

Linux, like many operating systems, is designed with security and access control in mind. One of the core mechanisms Linux uses for this purpose is **UID (User Identifier)** and **GID (Group Identifier)**. These identifiers are essential for managing user permissions and controlling access to both files and processes.

## What is UID?

The **UID** is a unique number assigned to each user on a Linux system. When a user logs in, the system associates the user’s username with their UID. This UID helps the system identify users and determine their permissions.

- **Root User**: The root user, who has full administrative privileges, is assigned the special UID of **0**. This grants root unrestricted access to the entire system.

- **Regular Users**: Other users on the system are assigned unique UIDs starting from **1**. Each user gets a different UID, and this ensures every user is uniquely identifiable by the system.

The **UID** is critical for ensuring that users only have access to the files and processes that they are permitted to.

## What is GID?

While the **UID** identifies individual users, the **GID** is used to manage groups of users. In Linux, users can belong to one or more groups, and each group is assigned a unique GID. This helps manage access to shared resources among multiple users.

- **System Groups**: Some groups are created automatically by Linux for system purposes, with predefined GIDs.

- **User Groups**: When a user is created, they are assigned to a primary group, which has its own unique GID. Users can also be part of additional groups, allowing for more flexible access control.

The **GID** is mainly used to control access to files and directories that are shared among users in the same group. For example, if multiple users are part of the same group, they can be granted permissions to access files associated with that group.


## UID in process

When a process is created (via `fork()` or `clone()`), it inherits the **real UID** from its parent. This real UID typically remains fixed throughout the life of the process unless explicitly changed. However, processes also use other UID types (like **effective UID**, **saved UID**, and **filesystem UID**) to handle permissions and privilege changes.

### Types of UIDs in a Process

**Real UID (UID)**
- The **real UID** identifies the user who owns the process.
- It is set when the process is created and usually does not change.
- Used to track the original owner of the process.

**Effective UID (EUID)**
- The **EUID** determines the privileges the process currently has.
- It can differ from the real UID, allowing processes to perform tasks with different privileges. For example, system programs may temporarily set their EUID to 0 (root) to perform privileged operations. After completing the task, the EUID can be reverted to the real UID or the saved UID.

**Filesystem UID (FSUID)**
- The FSUID is used specifically for filesystem access checks.
- It allows a process to access files with different ownership while keeping its real UID and EUID unchanged.
- This separation ensures that file access permissions can be managed independently of other process-level privileges.

**Saved UID (SUID)**
- The saved UID stores the EUID before it is changed.
- This allows a process to temporarily switch its EUID to perform a privileged operation (e.g., assume root privileges) and then revert back to its previous EUID.
- This mechanism is critical for security, as it limits the duration for which a process operates with elevated privileges.

### How UID represent in kernel

In the Linux kernel, UID and GID information are stored in `struct cred` that are part of the `struct task_struct`. These structures track a process's ownership and help enforce the correct permissions when interacting with resources.

Here’s how UID and GID are represented in the kernel:

```c
/* include/linux/sched.h */
struct task_struct {
    /* ... */
    /* Process credentials: */
	/* Tracer's credentials at attach: */
	const struct cred __rcu		*ptracer_cred;
	/* Objective and real subjective task credentials (COW): */
	const struct cred __rcu		*real_cred;
	/* Effective (overridable) subjective task credentials (COW): */
	const struct cred __rcu		*cred;
    /* ... */
}

/* include/linux/cred.h */
struct cred {
    /* ... */
    kuid_t		uid;		/* real UID of the task */
	kgid_t		gid;		/* real GID of the task */
	kuid_t		suid;		/* saved UID of the task */
	kgid_t		sgid;		/* saved GID of the task */
	kuid_t		euid;		/* effective UID of the task */
	kgid_t		egid;		/* effective GID of the task */
	kuid_t		fsuid;		/* UID for VFS ops */
	kgid_t		fsgid;		/* GID for VFS ops */
	unsigned	securebits;	/* SUID-less security management */
    /* ... */
}
```

# Capabilities

In traditional UNIX systems, processes are divided into two types based on their EUID:

- **Privileged processes**: EUID 0 (root), which can bypass all permission checks.
- **Unprivileged processes**: Non-zero EUID, subject to permission checks based on their EUID, EGID, and group memberships.

This simple model works but gives root users unrestricted access, posing security risks if exploited.

To address this, Linux kernel 2.2 introduced **capabilities**, breaking down root's all-powerful privileges into smaller, specific permissions, like bypassing file checks or changing system time.

Each thread can have its own capabilities, allowing processes to use only the permissions they truly need, enhancing security by following the principle of least privilege.

## Capabilities in Practice

Capabilities are implemented as bitmaps, with each bit representing a specific capability. A process's capabilities can exist in the following states:

- **Permitted**: The capabilities that a process is allowed to use.
- **Effective**: The subset of permitted capabilities that are currently active and in use.
- **Inheritable**: Capabilities that can be passed to child processes upon execution.

For practical management of capabilities, Linux provides tools such as `getcap` and `setcap`:

```bash
# By default, changing the hostname requires root privileges
$ hostname new
hostname: you must be root to change the host name

# Verify that "hostname" has no capabilities assigned
$ getcap `which hostname`

# Assign CAP_SYS_ADMIN capability to "hostname"
$ sudo setcap cap_sys_admin+ep `which hostname`

# Confirm the capability has been applied
$ getcap `which hostname`
/usr/bin/hostname = cap_sys_admin+ep

# Try and verify the hostname has been updated
$ hostname new
$ hostname
new
```

# User Namespace

User namespaces are a Linux kernel feature that isolate security-related identifiers and attributes, such as **UID**, **GID**, **root directories**, **kernel keys**, and **capabilities**. This allows processes to have separate, independent user-related credentials within the namespace, distinct from those in the global system.

The key concept is that within a user namespace, a process can have a different UID and GID compared to its parent namespace. For instance, a process might run as an unprivileged user in the parent namespace but be mapped to UID 0 (root) within the user namespace. This enables the process to perform privileged operations inside the namespace while retaining its unprivileged status outside, improving isolation and security.


## UID and GID Mapping

Each process in a user namespace can configure UID and GID mappings through these files:

- `/proc/[pid]/uid_map`
- `/proc/[pid]/gid_map`

By default, these files are empty. If no mapping is defined, unmapped IDs are translated to the **nobody** ID, determined by `/proc/sys/kernel/overflowuid` and `/proc/sys/kernel/overflowgid`.

```bash
$ unshare --user
$ cat /proc/$$/status | grep Cap
CapInh: 0000000000000000
CapPrm: 0000000000000000
CapEff: 0000000000000000
CapBnd: 000001ffffffffff
CapAmb: 0000000000000000
$ id
uid=65534(nobody) gid=65534(nobody) groups=65534(nobody)
echo '0 1000 1' > /proc/$$/uid_map
-bash: echo: write error: Operation not permitted
```
To map IDs between the parent and user namespace, specific rules must be followed:

- The UID/GID mapping must include the process's effective user ID or group ID in the parent namespace.
- For gid_map, the `setgroups` system call must be disabled by writing deny to `/proc/[pid]/setgroups` before configuring the mapping.
- Only the namespace creator or a process with the same effective UID can write these mappings.

```bash
$ sudo setcap cap_sys_admin+ep `which bash`
$ unshare --user
$ cat /proc/$$/status | grep Cap
CapInh: 0000000000000000
CapPrm: 0000000000200000
CapEff: 0000000000200000
CapBnd: 000001ffffffffff
CapAmb: 0000000000000000
$ echo '0 1000 1' > /proc/$$/uid_map
$ echo 'deny' > /proc/$$/setgroups
$ echo '0 1000 1' > /proc/$$/gid_map
$ id
uid=0(root) gid=0(root) groups=0(root),65534(nobody)
```