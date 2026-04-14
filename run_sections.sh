#!/usr/bin/env bash

set -euo pipefail

usage() {
  cat << 'EOF'
Usage: ./run_sections.sh <target>

Targets:
  deps             Install required Ubuntu packages
  build            Compile all binaries
  0                Run Section 0 baseline commands
  1                Run non-PIE layout binary
  1-pie            Run PIE layout binary twice
  2                Run demand paging program
  2-perf           Run page-fault count with perf
  3                Run pagemap program (sudo required)
  4                Run copy-on-write program
  5                Run shared library checks and hello binaries
  5-libc-share     Count processes sharing libc (sudo required)
  6                Run alloc_trace and strace brk/mmap
  7                Run fragmentation program
  8                Run swap pressure program
  8-safe           Run safer swap pressure program (30% RAM)
  8-vmstat         Monitor system memory activity
  9-small          Run working_set with 64 MB working set
  9-large          Run working_set with 512 MB working set
  9-meminfo        Print Active/Inactive counters
  10               Run full thrash benchmark
  10-safe          Run safer thrash benchmark (base 3000MB, 50000 accesses)
  swappiness       Print vm.swappiness value
  clean            Remove compiled binaries
EOF
}

if [[ ${1:-} == "" ]]; then
  usage
  exit 1
fi

case "$1" in
  deps)
    sudo apt update
    sudo apt install -y gcc build-essential linux-tools-common "linux-tools-$(uname -r)" sysstat strace
    ;;
  build)
    make clean
    make all
    ;;
  0)
    free -m
    echo
    getconf PAGESIZE
    echo
    awk '/MemTotal/ {print $2 / 4}' /proc/meminfo
    ;;
  1)
    ./layout
    ;;
  1-pie)
    ./layout_pie
    echo
    ./layout_pie
    ;;
  2)
    ./demand
    ;;
  2-perf)
    if ! perf stat -e page-faults ./demand; then
      echo
      echo "Non-root perf blocked by kernel policy. Retrying with sudo..."
      sudo perf stat -e page-faults ./demand
    fi
    ;;
  3)
    sudo ./pagemap
    ;;
  4)
    ./cow
    ;;
  5)
    ldd /bin/bash
    echo
    ls -lh hello_dynamic hello_static
    echo
    LD_DEBUG=libs ./hello_dynamic 2>&1 | head -30
    echo
    cat /proc/self/maps | grep libc || true
    ;;
  5-libc-share)
    sudo grep 'libc' /proc/*/maps 2>/dev/null | awk '{print $NF}' | sort -u | head -5
    echo "---"
    echo "Processes sharing libc:"
    sudo grep 'libc' /proc/*/maps 2>/dev/null | awk -F/ '{print $3}' | sort -un | wc -l
    ;;
  6)
    ./alloc_trace
    echo
    strace -e brk,mmap ./alloc_trace 2>&1 | tail -20
    ;;
  7)
    ./fragment
    ;;
  8)
    ./swap_pressure
    ;;
  8-safe)
    ./swap_pressure 30
    ;;
  8-vmstat)
    vmstat 1
    ;;
  9-small)
    ./working_set 512 64 5
    ;;
  9-large)
    ./working_set 512 512 5
    ;;
  9-meminfo)
    grep -E 'Active|Inactive' /proc/meminfo
    ;;
  10)
    ./thrash
    ;;
  10-safe)
    ./thrash 3000 50000
    ;;
  swappiness)
    cat /proc/sys/vm/swappiness
    ;;
  clean)
    make clean
    ;;
  *)
    usage
    exit 1
    ;;
esac
