# OverlayFS

**OverlayFS** is a lightweight, high-performance **union file system** implemented in the Linux kernel. It allows multiple file systems, or "layers," to be combined into a single unified **view**. This is particularly useful for scenarios like containerized environments, where efficiency, modularity, and separation of read-write operations are essential.

## Core Concepts

OverlayFS operates by layering directories to create a merged view:

- **Lower Directory (lowerdir)**: Typically read-only, containing the base files or snapshots.
- **Upper Directory (upperdir)**: A writable directory where changes are recorded.
- **Work Directory (workdir)**: Used internally by OverlayFS for operations like copy-on-write.

Files from the upper directory override those in the lower directory, and any modifications or additions occur in the upper directory. Unmodified files in the lower directory remain untouched, ensuring data integrity.

### Copy-on-Write Mechanism

OverlayFS uses a **copy-on-write (CoW)** strategy:

When a file from the lower directory is modified, it is first copied to the upper directory. The modification is then applied to this copied file, leaving the lower directory unaltered.

## Testing

First, set up the required directory structure and files:

```bash
$ mkdir lowerdir upperdir workdir mergedir
$ touch lowerdir/file1 lowerdir/file2 upperdir/file1
$ echo "lower" > lowerdir/file1
$ echo "upper" > upperdir/file1
```

Mount the OverlayFS filesystem with `lowerdir`, `upperdir`, and `workdir`.
The `file1` in the `mergedir` view comes from the `upperdir`, overriding the `lowerdir` version.

```bash
$ sudo mount -t overlay overlay -o lowerdir=lowerdir,upperdir=upperdir,workdir=workdir mergedir

$ findmnt mergedir
TARGET                    SOURCE  FSTYPE  OPTIONS
/home/dadmin/tmp/mergedir overlay overlay rw,relatime,seclabel,lowerdir=lowerdir,upperdir=upperdir,workdir=workdir

$ tree mergedir/
mergedir/
├── file1
└── file2

$ cat mergedir/file1
upper
```

Create a new file in the `merged` directory, which written to `upperdir`, as expected.
```bash
$ touch mergedir/file3
$ find *dir -iname file3
mergedir/file3
upperdir/file3
```

Unmount the Overlay filesystem.

```bash
$ sudo umount mergedir
```
<br>

# Layers in Docker

A layer in a Docker image is a read-only incremental change based on the instructions in a Dockerfile. When a container is created from an image, Docker adds a writable layer, known as the container layer, on top of the read-only layers. This writable layer stores any modifications made during the container's runtime.

## Layers Creation

Layers are built when Docker processes a Dockerfile. Consider the following example:

```dockerfile
# syntax=docker/dockerfile:1

FROM ubuntu:22.04
LABEL org.opencontainers.image.authors="org@example.com"
COPY . /app
RUN make /app
RUN rm -r $HOME/.cache
CMD python /app/app.py
```

- `FROM ubuntu:22.04`: This creates a **layer** based on the official ubuntu:22.04 image.
- `LABEL`: Adds metadata about the image (such as the author's information). It does not create a new layer.
- `COPY . /app`: This command copies files from your Docker client's current directory to /app inside the image. This creates a **new layer**.
- `RUN make /app`: This command executes the `make` command to build your application, creating a **new layer** with the build output.
- `RUN rm -r $HOME/.cache`: Removes the `$HOME/.cache` directory, which results in another **new layer**.
- `CMD python /app/app.py`: Specifies the default command to run when the container starts. This only modifies the image's metadata and does not create a new layer.

## Layers Storage

Docker uses a **storage driver** to manage the layers of an image. These storage drivers determine how layers are stored and stacked on the host filesystem. Popular storage drivers include **OverlayFS**, **AUFS**, and **btrfs**, each with its own method for handling the layers and interacting with the container's filesystem.

### GraphDriver and RootFS

Docker uses **GraphDriver** to manage the physical storage of these layers. The GraphDriver organizes the read-only layers (which are part of the image) and the writable container layer. The writable layer is where changes made during container execution (such as creating new files or modifying existing ones) are stored. The choice of storage driver (e.g., OverlayFS, AUFS) determines how these layers are actually stored on the host filesystem.

On the other hand, **RootFS** provides a logical structure for these layers. It lists the layers in the order they are stacked, from the base image to any additional layers added during the build process. RootFS gives you a view of how these layers combine to form the final filesystem inside the container.

After building an image, you can inspect it with `docker inspect`:

```bash
$ docker inspect registry

[
    {
        # ... ...
        "GraphDriver": {
            "Data": {
                "LowerDir": "/var/lib/docker/overlay2/080d2473b990b38ea73e150dc6f97b609597d274201c19b96fb83420df9e2565/diff:/var/lib/docker/overlay2/dc9d5c073d64ac2b4bb026a9c680808d669999f9a8a697d4f448ca95a8b9d735/diff:/var/lib/docker/overlay2/989d8973c3ed4e8bce66cee29193e110ba9f92d74c89da6488f790ffc3a37e67/diff:/var/lib/docker/overlay2/ad6075addf199e0ba8a283195843e8a90451a63980c076aeb24b5598ae337521/diff",
                "MergedDir": "/var/lib/docker/overlay2/6f6724027d1ebf166c1e919118d09aead5fd28338718e9cdb527f7a6beeccd42/merged",
                "UpperDir": "/var/lib/docker/overlay2/6f6724027d1ebf166c1e919118d09aead5fd28338718e9cdb527f7a6beeccd42/diff",
                "WorkDir": "/var/lib/docker/overlay2/6f6724027d1ebf166c1e919118d09aead5fd28338718e9cdb527f7a6beeccd42/work"
            },
            "Name": "overlay2"
        },
        "RootFS": {
            "Type": "layers",
            "Layers": [
                "sha256:f9fc769d335082905a3e43c83f2a6d11b413556f9d156ab0fbe3166d256c0d99",
                "sha256:96a01afdff54b93b2a2f6750e63c0a74eb153f199296f0447fc11f379e8f9d22",
                "sha256:028476fc17e093cfb1eef878ce7d357d931873b7a55b40bacd08623ac76566f9",
                "sha256:d28d4e94d6f3a6fa6ae5a0c8c5a34d40056657f7e796c636f326141b667c3eaf",
                "sha256:7827cbf539f47a6534bf74d48ced026dbfa0d3d10f3903374fbe9c926b80ab51"
            ]
        },
        "Metadata": {
            "LastTagTime": "0001-01-01T00:00:00Z"
        }
    }
]
```
