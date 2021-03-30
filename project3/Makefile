# makefile for Virtual Memeory Unit (MMU)
#
# usage: make mmu

CC=gcc
CFLAGS=-Wall

clean:
	rm -rf *.o
	rm -rf mmu
	
mmu:  VirtualMemoryUnit.o
	$(CC) $(CFLAGS) -o mmu VirtualMemoryUnit.o

VirtualMemoryUnit.o: VirtualMemoryUnit.c
	$(CC) $(CFLAGS) -c VirtualMemoryUnit.c