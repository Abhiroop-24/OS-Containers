# Lightweight Multi-Container Runtime

This repository implements a Linux-based supervised multi-container runtime in C,
with a kernel module for memory monitoring.

## Components

- `engine.c`: user-space supervisor and CLI (`supervisor`, `start`, `run`, `ps`, `logs`, `stop`)
- `monitor.c`: kernel module that tracks container memory and applies soft/hard limits
- `monitor_ioctl.h`: shared ioctl interface between user-space and kernel-space
- `cpu_hog.c`, `memory_hog.c`, `io_pulse.c`: workload binaries for testing
- `environment-check.sh`: environment validation helper
- `Makefile`: user-space + kernel-module build targets

## Requirements

- Ubuntu Linux VM (kernel-module support required)
- `build-essential`
- `linux-headers-$(uname -r)`

Install dependencies:

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r)
```

## Build

Build user binaries only:

```bash
make ci
```

Build user binaries + kernel module:

```bash
make all
```

## Quick Run

1. Build and load kernel module:

```bash
make all
sudo insmod monitor.ko
ls -l /dev/container_monitor
```

2. Prepare container root filesystems (example):

```bash
mkdir -p rootfs-base
wget https://dl-cdn.alpinelinux.org/alpine/v3.20/releases/x86_64/alpine-minirootfs-3.20.3-x86_64.tar.gz
tar -xzf alpine-minirootfs-3.20.3-x86_64.tar.gz -C rootfs-base
cp -a rootfs-base rootfs-alpha
cp -a rootfs-base rootfs-beta
```

3. Copy workloads into each rootfs:

```bash
sudo cp memory_hog cpu_hog io_pulse rootfs-alpha/
sudo cp memory_hog cpu_hog io_pulse rootfs-beta/
```

4. Start supervisor (terminal 1):

```bash
sudo ./engine supervisor ./rootfs-base
```

5. Run commands (terminal 2):

```bash
sudo ./engine start alpha ./rootfs-alpha /memory_hog --soft-mib 40 --hard-mib 64
sudo ./engine start beta ./rootfs-beta /cpu_hog --nice 10
sudo ./engine ps
sudo ./engine logs alpha
sudo ./engine stop alpha
```

6. Cleanup:

```bash
sudo rmmod monitor
make clean
```

## Notes

- This project is Linux VM oriented; kernel module testing is not supported in WSL.
- `dmesg` shows soft/hard memory-limit monitor events from `monitor.ko`.
