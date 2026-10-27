# Default module to build if none is specified.
# You can change this to your preferred default (e.g., rhel_proc_info)
KMOD ?= hello_module

# obj-m specifies the kernel module(s) to build.
# It now uses the value of the KMOD variable.
obj-m := $(KMOD).o

# Directory where your source files are located (current directory)
PWD := $(shell pwd)

all:
	# -C: Change to the directory of the kernel build system
	# /lib/modules/$(shell uname -r)/build: Path to the kernel build directory for the current kernel
	# M=$(PWD): Tell the kernel build system where our module source code is
	# modules: The target to build kernel modules
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	# Same logic as 'all', but calls the 'clean' target in the kernel build system
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

.PHONY: all clean
