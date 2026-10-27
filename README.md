# Linux Kernel Modules: A Beginner's Guide

This repository contains examples and instructions for learning how to create and manage Linux kernel modules, specifically tailored for Red Hat Enterprise Linux (RHEL) systems.

## What is a Linux Kernel Module?

A Linux Kernel Module (LKM) is an object file that can be loaded and unloaded into the kernel of a running operating system without rebooting the system. LKMs extend the functionality of the kernel. They are typically used for:

*   **Device Drivers:** Allowing the kernel to communicate with hardware.
*   **Filesystems:** Adding support for new file systems.
*   **System Calls:** Implementing new system calls.
*   **Networking Protocols:** Extending network capabilities.

This guide will walk you through creating two simple modules: a classic "Hello World" and a module that interacts with the `/proc` filesystem.

## Prerequisites

To compile kernel modules on a RHEL system, you need the kernel development headers and tools.

1.  **Ensure your system is up to date:**
    ```bash
    sudo yum update -y
    ```
2.  **Install the kernel-devel package:** This package provides the necessary kernel headers and Makefiles for building external kernel modules that match your currently running kernel version.
    ```bash
    sudo yum install kernel-devel-$(uname -r)
    ```
    The `$(uname -r)` command ensures you install the `kernel-devel` package for your exact running kernel version.

## Part 1: The "Hello World" Module

This is the simplest possible kernel module. It demonstrates the basic structure: an initialization function (called when the module loads) and an exit function (called when the module unloads). Both functions simply print a message to the kernel log.

### 1.1. `hello_module.c`

Refer to the source file [`hello_module.c`](./hello_module.c) and take note of the following:

*   `module.h` and `kernel.h`: Essential headers for kernel module development.
*   `__init` and `__exit`: Macros that mark functions as initialization and exit routines, respectively. The kernel can free memory associated with `__init` functions after module loading, and `__exit` functions are only linked if the module is built as loadable.
*   `printk(KERN_INFO ...)`: The kernel's equivalent of `printf()`. `KERN_INFO` is a log level, indicating informational messages.
*   `module_init()` and `module_exit()`: Register your `init` and `exit` functions with the kernel.
*   `MODULE_LICENSE`, `MODULE_AUTHOR`, `MODULE_DESCRIPTION`, `MODULE_VERSION`: Macros that provide metadata about your module. `MODULE_LICENSE("GPL")` is crucial; using an open-source license like GPL prevents the kernel from being "tainted" when your module is loaded.

### 1.2. `Makefile`

Now look at the [`Makefile`](./Makefile) in the same directory. It defines how `hello_module.c` will be compiled to build our kernel module.

### 1.3. Compile, Load, and Test

That's all we need. Follow these steps to try it out:

1.  **Navigate to the code directory, for example:**
    ```bash
    cd ~/hello_module
    ```
    Specify the directory that you cloned this repo to.
2.  **Compile the module:**
    ```bash
    make
    ```
    This will create the kernel object file name `hello_module.ko`.
3.  **Load the module:**
    ```bash
    sudo insmod hello_module.ko
    ```
    The `insmod` command reads a kernel object file to insert a module into the kernel.
4.  **Check kernel messages:** View the last few kernel messages to see your module's output:
    ```bash
    sudo dmesg -T | tail
    ```
    You should see: `[ ... ] Hello, World! Your RHEL kernel module have been loaded`
5.  **Unload the module:**
    ```bash
    sudo rmmod hello_module
    ```
    The `rmmod` command removes a module from the kernel. Note that you provide the module name without the `.ko` suffix here.
6.  **Check kernel messages again:**
    ```bash
    sudo dmesg -T | tail
    ```
    You should see: `[ ... ] Goodbye, World! Your RHEL kernel module is going away`

## Part 2: Interacting with `/proc` - Getting Process Info

With the next module, we demonstrate how to create a custom entry in the `/proc` filesystem. When a user writes to this `/proc` entry, our module will log information about the process that performed the write. This is a common way for user-space programs to communicate with and trigger actions within kernel modules.

### 2.1. `rhel_proc_info.c`

Have a look at the source file [`rhel_proc_info.c`](./rhel_proc_info.c) and notice the following: 

*   `proc_fs.h`: Provides functions and structures for interacting with the `/proc` filesystem.
*   `sched.h`: Contains definitions for process scheduling, including the `task_struct` and the `current` macro.
*   `proc_create()`: Used to create a new entry in `/proc`.
*   `rhel_proc_write()`: This is the handler function that gets called when a user-space program writes to `/proc/rhel_proc_info`. Crucially, `current` points to the `task_struct` of the process performing the write.

### 2.2. `Makefile`

We will use the same `Makefile` as before. It is configured to build `hello_module` by default, but we can tell it to build our new `rhel_proc_info` module instead.

### 2.3. Compile, Load, and Test

Follow these steps much like what we did before:

1.  **Compile the module:**
    ```bash
    make KMOD=rhel_proc_info
    ```
    We are overriding the KMOD variable, so this will produce `rhel_proc_info.ko`.
2.  **Load the module:**
    ```bash
    sudo insmod rhel_proc_info.ko
    ```
    Verify it loaded successfully using `dmesg` and by checking for `/proc/rhel_proc_info`:
    ```bash
    sudo dmesg -T | tail
    ls -l /proc/rhel_proc_info
    ```
3.  **Trigger the module from user-space:**
    Open another terminal. In this new terminal, execute commands that write to `/proc/rhel_proc_info`. Remember that this file requires root privileges to write to.
    ```bash
    # Example 1: Trigger with 'sh' (from sudo sh -c '...')
    sudo sh -c 'echo "This is a test" > /proc/rhel_proc_info'

    # Example 2: Trigger with 'bash' (from sudo bash -c '...')
    sudo bash -c 'echo "Another trigger" > /proc/rhel_proc_info'

    # Example 3: Trigger with 'python3'
    sudo python3 -c 'open("/proc/rhel_proc_info", "w").write("python trigger")'
    ```
5.  **Check kernel messages:** In your original terminal, check `dmesg` again:
    ```bash
    dmesg | tail
    ```
    You should see messages similar to:
    ```
    [ ... ] rhel_proc_info: Triggered by process 'sh' (PID: 12345)
    [ ... ] rhel_proc_info: Triggered by process 'bash' (PID: 67890)
    [ ... ] rhel_proc_info: Triggered by process 'python3' (PID: 23456)
    ```
    (The PIDs and timestamps will vary.)
6.  **Unload the module:**
    ```bash
    sudo rmmod rhel_proc_info
    ```
    Use `dmesg` to confirm the module unloaded. Also observe that the `/proc/rhel_proc_info` entry is removed.

## Important Considerations

*   **Kernel Tainting:** If a kernel module is loaded that does not have an open-source license (e.g., "GPL"), the kernel is marked as "tainted." This is a warning that an unsupported module is running, and Red Hat Support might ask you to unload such modules if you encounter issues. Always use an appropriate license like `GPL`.
*   **System Stability:** Kernel modules run in kernel-space, meaning they have full control over the system. A bug in a kernel module can lead to a kernel panic (system crash), data corruption, or security vulnerabilities. Always develop and test modules carefully, preferably in a virtual machine.
*   **`sudo` for Module Operations:** Loading and unloading kernel modules requires root privileges, hence the use of `sudo` with `insmod` and `rmmod`.
*   **Debugging:** `printk()` and `dmesg` are your primary tools for debugging kernel modules. There are more advanced techniques (like `kgdb`), but `printk` is usually sufficient for simple issues.
*   **Kernel Version Dependency:** Kernel modules are specific to the kernel version they are compiled against. If you update your kernel, you will typically need to recompile your modules. The `$(uname -r)` trick in the `Makefile` helps ensure this compatibility.

## Further Learning

Once you're comfortable with these basics, you can explore:

*   **Character Devices:** Creating `/dev` entries to expose device functionality to user-space.
*   **Module Parameters:** Allowing configuration of your module at load time.
*   **Kernel Timers:** Performing actions at regular intervals.
*   **Workqueues and Tasklets:** Deferring work to prevent blocking the kernel.
*   **Memory Allocation:** Using kernel-specific memory allocation routines (`kmalloc`, `vmalloc`).

Happy kernel hacking!
