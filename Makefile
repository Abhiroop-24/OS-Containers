CC = gcc
CFLAGS = -O0 -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L

TARGETS = layout layout_pie demand pagemap cow hello_dynamic hello_static alloc_trace fragment swap_pressure working_set thrash

all: $(TARGETS)

layout: layout.c
	$(CC) $(CFLAGS) -no-pie -o $@ $<

layout_pie: layout.c
	$(CC) $(CFLAGS) -o $@ $<

demand: demand.c
	$(CC) $(CFLAGS) -o $@ $<

pagemap: pagemap.c
	$(CC) $(CFLAGS) -o $@ $<

cow: cow.c
	$(CC) $(CFLAGS) -o $@ $<

hello_dynamic: hello.c
	$(CC) $(CFLAGS) -o $@ $<

hello_static: hello.c
	$(CC) $(CFLAGS) -static -o $@ $<

alloc_trace: alloc_trace.c
	$(CC) $(CFLAGS) -o $@ $<

fragment: fragment.c
	$(CC) $(CFLAGS) -o $@ $<

swap_pressure: swap_pressure.c
	$(CC) $(CFLAGS) -o $@ $<

working_set: working_set.c
	$(CC) $(CFLAGS) -o $@ $<

thrash: thrash.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
