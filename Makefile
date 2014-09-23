all:cpu memory

cpu:cpu.c
	gcc -std=c99 -pthread -O2 -funroll-loops cpu.c -o cpu
memory:mem.c
	gcc -std=c99 -pthread mem.c -o memory

clean:
	rm cpu memory
