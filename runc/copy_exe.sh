#!/bin/bash

# Check if the input parameter is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_executable>"
    exit 1
fi

# The path to the executable file
# executable="$1"
executable=$(which "$1")

# Check if the file exists and is executable
if [ ! -x "$executable" ]; then
    echo "Error: '$executable' is not a valid executable."
    exit 1
fi

# Create the rootfs directory structure (if it doesn't already exist)
rootfs="./rootfs"
lower_dir="$rootfs/lower_dir"
upper_dir="$rootfs/upper_dir"
work_dir="$rootfs/work_dir"
merged_dir="$rootfs/merged_dir"
bin_dir="$lower_dir/bin"
lib_dir="$lower_dir/lib64"
proc_dir="$lower_dir/proc"
etc_dir="$lower_dir/etc"
root_dir="$lower_dir/root"

mkdir -p "$merged_dir" "$upper_dir" "$work_dir" "$bin_dir" "$lib_dir" "$proc_dir" "$etc_dir" "$root_dir" 

cp /etc/profile "$etc_dir"
cp /etc/bashrc "$etc_dir"
echo 'export PS1="[\u@\h \W]\$ "' >> "$root_dir/.bashrc"
cp /etc/passwd "$etc_dir"
cp /lib64/libnss_files.so.2 "$lib_dir"

# Copy the executable to the rootfs/bin directory
executable_name=$(basename "$executable")
cp "$executable" "$bin_dir/$executable_name"

# Use ldd to find the dependent libraries and copy them to rootfs/lib
# echo "Fetching dependencies for $executable..."
ldd "$executable" | while read -r line; do
    # Match valid library paths (filter out lines without actual paths, like "linux-vdso.so.1")
    if [[ "$line" =~ (/.*)\ (.*) ]]; then
        lib_path="${BASH_REMATCH[1]}"
      
        # If the library file exists and has not been copied already, copy it to the lib directory
        if [ -f "$lib_path" ] && [ ! -f "$lib_dir/$(basename "$lib_path")" ]; then
            # echo "Copying library: $lib_path"
            cp "$lib_path" "$lib_dir/"
        fi
    fi
done

# echo "Setup complete. The executable and its libraries are in '$lower_dir'."
