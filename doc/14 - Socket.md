# Socket

A socket is like a connection point that allows programs to talk to each other. There are two main types: 
- **Internet domain sockets**: are used for communication between programs on different machines over a network
- **UNIX domain sockets**: are meant for communication between programs running on the same machine.

The typical use case for sockets is the **client-server model**, where a client communicates with a server over a network or locally.

<img src="img/14-1.png" alt="socket_communication" width="350">

## Socket system calls

Here is a list of common Linux socket functions:

- `socket()`: Creates a new socket.
- `bind()`: Binds a socket to an address and port.
- `listen()`: Prepares a socket to listen for incoming connections (server-side).
- `accept()`: Accepts an incoming connection on a listening socket (server-side).
- `connect()`: Initiates a connection to a remote server (client-side).
- `send()`: Sends data through a socket.
- `recv()`: Receives data from a socket.
- `close()`: Closes a socket.
- `setsockopt()`: Sets options for a socket (e.g., buffer size, timeouts).
- `getsockopt()`: Retrieves socket options.

<br>

# Epoll

In Linux, **epoll** is an event notification mechanism that allows an application to efficiently monitor multiple file descriptors to determine if I/O operations can be performed.

The file descriptors monitored by epoll can operate in either **Level-Triggered** (default) or **Edge-Triggered** mode.

- **Level-Triggered (LT)**: The file descriptor triggers an event as long as the event condition is true. If the condition persists after the event is processed, the file descriptor will continue to trigger events.
- **Edge-Triggered (ET)**: The file descriptor triggers an event only when its status changes (i.e., when the condition transitions from false to true).

When building a server with epoll, it's recommended to set the file descriptors to non-blocking mode (using the `O_NONBLOCK` flag) and enable Edge-Triggered mode (using the `EPOLLET` flag). I/O event handling should be completed only after `read()` or `write()` returns `EAGAIN`, indicating that the operation should be retried later.

## Epoll system calls


The following system calls are provided to create and manage an **epoll** instance:

- `epoll_create()`: Creates a new epoll instance.
- `epoll_ctl()`: Adds, modifies, or removes file descriptors from the epoll instance.
- `epoll_wait()`: Waits for events on the file descriptors being monitored by the epoll instance.