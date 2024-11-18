# Hostname

A hostname is a label assigned to a machine on a network to uniquely identify it. It is primarily used in network communications to establish a recognizable identity for a system. Hostnames can be combined with domain names to form a **Fully Qualified Domain Name (FQDN)**, which specifies the machine's exact location within the domain hierarchy.

Linux maintains the hostname as a **kernel attribute**. This attribute is transient and represents the system's running hostname. However, there are also persistent settings stored in files `/etc/hostname`, which ensure the hostname is preserved across reboots.

## System Function for Hostname Management.

Linux provides system calls to interact with the hostname: `gethostname` and `sethostname`. These functions interact directly with the kernel.

### `gethostname`

This system call retrieves the current hostname from the kernel.

```c
int gethostname(char *name, size_t len);
```

- `name`: A buffer where the hostname will be stored.
- `len`: The size of the buffer

### `sethostname`

This system call sets the system's hostname. It requires root privileges.

```c
int sethostname(const char *name, size_t len);
```
- `name`: The new hostname.
- `len`: The length of the hostname.

<br>

# UTS Namespace

The **UTS (UNIX Time-sharing System)** namespace isolates two kernel attributes:

- **Hostname**: identifies a machine on a network.

- **NIS (Network Information System) Domain Name**: was historically used to group systems for resource sharing, but it is largely obsolete in modern systems.

Using the UTS namespace, processes can have unique hostnames and NIS domain names independent of other processes. This is particularly useful in containerized environments.

## `unshare` system call

The `unshare` system call is key to namespace management. 

```c
int unshare(int flags);
```

- `flags`: Specifies the namespace to unshare. For UTS namespace isolation, use `CLONE_NEWUTS`.