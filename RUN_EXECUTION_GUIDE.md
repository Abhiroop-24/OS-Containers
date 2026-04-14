# Memory Management Assignment - Run Guide

Follow this in exact order.

## Platform Note

This lab uses Linux-only interfaces (`/proc`, `fork`, `mmap`, `vmstat`, `perf`, `strace`).

- Linux users: run directly.
- Windows users: run inside WSL2 Ubuntu or an Ubuntu VM.

## 1. One-Time Setup (Run Once)

```bash
cd <your-repository-folder>
# Example: cd OS-Memory-Management

# Optional: custom prompt with your last 3 SRN digits (Linux shell only)
export PS1="042\$ "

chmod +x run_sections.sh
./run_sections.sh deps
./run_sections.sh build
```

## 2. Screenshot 0 - Baseline Memory

```bash
free -m
getconf PAGESIZE
awk '/MemTotal/ {print $2 / 4}' /proc/meminfo
```

Capture: keep `free -m` output clearly visible.

## 3. Screenshot 1 - Virtual Address Layout

```bash
./layout
```

Capture: full output with Text/Rodata/Data/BSS/Heap/Stack and filtered maps.

Also run for Q1.3:

```bash
./layout_pie
./layout_pie
```

## 4. Screenshot 2 - Demand Paging

```bash
./demand
```

Capture: all four blocks `[1..4]` with VmSize/VmRSS.

Also run for Q2.2:

```bash
./run_sections.sh 2-perf
```

## 5. Screenshot 3 - Pagemap Present Bits

```bash
sudo ./pagemap
```

Capture: both before-touch and after-touch outputs.

## 6. Screenshot 4 - Copy-on-Write

```bash
./cow
```

Capture: read-only fault count and write-pass fault count.

## 7. Screenshot 5 - Shared Libraries

```bash
ldd /bin/bash
ls -lh hello_dynamic hello_static
LD_DEBUG=libs ./hello_dynamic 2>&1 | head -30
cat /proc/self/maps | grep libc
```

Capture at minimum:
- `ls -lh hello_dynamic hello_static`
- first 30 lines of `LD_DEBUG=libs`

Also run for Q5.3:

```bash
sudo grep 'libc' /proc/*/maps 2>/dev/null | awk '{print $NF}' | sort -u | head -5
echo "---"
echo "Processes sharing libc:"
sudo grep 'libc' /proc/*/maps 2>/dev/null | awk -F/ '{print $3}' | sort -un | wc -l
```

## 8. Screenshot 6 - brk vs mmap

```bash
./alloc_trace
strace -e brk,mmap ./alloc_trace 2>&1 | tail -20
```

Capture: alloc_trace output and strace tail showing brk/mmap.

## 9. Screenshot 7 - Fragmentation

```bash
./fragment
```

Capture: full output including external and internal fragmentation.

## 10. Screenshot 8 - Swap Pressure (Two Terminals)

Terminal A:

```bash
vmstat 1
```

Terminal B:

```bash
swapon --show
free -m
./swap_pressure
```

Capture: vmstat activity during run and final VmRSS/VmSwap from program output.

If swap is missing, create once:

```bash
sudo fallocate -l 512M /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
swapon --show
```

Stop vmstat after run with `Ctrl+C`.

## 11. Screenshot 9 - Working Set Timing

```bash
grep -E 'Active|Inactive' /proc/meminfo
./working_set 512 64 5
grep -E 'Active|Inactive' /proc/meminfo
./working_set 512 512 5
grep -E 'Active|Inactive' /proc/meminfo
```

Capture: timings for both 64 MB and 512 MB runs.

## 12. Screenshot 10 - Thrashing (Two Terminals)

Terminal A:

```bash
vmstat 1
```

Terminal B:

```bash
./thrash
```

Capture: throughput table and vmstat during slowdown.

If laptop becomes unstable:

```bash
./thrash 3000 50000
```

Stop vmstat after completion with `Ctrl+C`.

## 13. Cleanup

```bash
make clean
```

## Quick Checklist

- [ ] Screenshot 0 done
- [ ] Screenshot 1 done
- [ ] Screenshot 2 done
- [ ] Screenshot 3 done
- [ ] Screenshot 4 done
- [ ] Screenshot 5 done
- [ ] Screenshot 6 done
- [ ] Screenshot 7 done
- [ ] Screenshot 8 done
- [ ] Screenshot 9 done
- [ ] Screenshot 10 done
