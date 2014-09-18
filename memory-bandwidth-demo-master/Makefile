memory_profiler: main.c monotonic_timer.c functions.c
	gcc-4.8 -O3 $^ -o $@ -fopenmp -DWITH_OPENMP

.PHONY: run
run: memory_profiler
	./memory_profiler
