# OS Unit 3 - Banana (Memory Management)

This project implements the Memory Management experiments from the Banana lab:

- https://github.com/pestechnology/OS-U3-BANANA

## What Is Implemented

- Section 1: Virtual address space layout using code/data/bss/heap/stack addresses
- Section 2: Demand paging with VmSize and VmRSS changes
- Section 3: Page table present-bit observation through pagemap
- Section 4: Copy-on-write behavior with fork and page-fault counts
- Section 5: Dynamic vs static linking and shared libc mapping
- Section 6: malloc behavior via brk vs mmap with strace
- Section 7: External and internal fragmentation demonstration
- Section 8: Swap pressure and process memory status
- Section 9: Working set timing comparison
- Section 10: Thrashing throughput benchmark under increasing memory pressure

## Project Files

- layout.c
- demand.c
- pagemap.c
- cow.c
- hello.c
- alloc_trace.c
- fragment.c
- swap_pressure.c
- working_set.c
- thrash.c
- Makefile
- run_sections.sh

## Setup

Platform: Linux (Ubuntu). If you are on Windows, use WSL2 Ubuntu or an Ubuntu VM.

Install dependencies:

```bash
cd <your-repository-folder>
chmod +x run_sections.sh
./run_sections.sh deps
```

Compile all programs:

```bash
./run_sections.sh build
```

## How To Run

Run each section directly:

```bash
./run_sections.sh 0
./run_sections.sh 1
./run_sections.sh 1-pie
./run_sections.sh 2
./run_sections.sh 2-perf
./run_sections.sh 3
./run_sections.sh 4
./run_sections.sh 5
./run_sections.sh 5-libc-share
./run_sections.sh 6
./run_sections.sh 7
./run_sections.sh 8
./run_sections.sh 8-safe
./run_sections.sh 9-meminfo
./run_sections.sh 9-small
./run_sections.sh 9-large
./run_sections.sh 10-safe
./run_sections.sh swappiness
```

Full stress run:

```bash
./run_sections.sh 10
```

For swap/thrashing monitoring in parallel:

```bash
# Terminal 1
./run_sections.sh 8-vmstat

# Terminal 2
./run_sections.sh 8
./run_sections.sh 10-safe
```

## Notes

- Section 3 and some Section 5 commands require `sudo`.
- 10-safe is recommended for laptop stability.
- If static linking fails on your machine, install glibc static support packages.
