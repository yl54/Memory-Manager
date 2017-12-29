clean: 
	rm -f *.o bench quick_test

bench: bench.o print_heap.o mem_utils.o get_mem_stats.o getmem.o freemem.o
	gcc -Wall -g -std=c11 -o bench bench.o print_heap.o mem_utils.o get_mem_stats.o getmem.o freemem.o

quick_test: quick_test.o print_heap.o mem_utils.o get_mem_stats.o getmem.o freemem.o
	gcc -Wall -g -std=c11 -o quick_test quick_test.o print_heap.o mem_utils.o get_mem_stats.o getmem.o freemem.o

freemem.o: freemem.c mem_impl.h mem.h
	gcc -Wall -g -std=c11 -c freemem.c

getmem.o: getmem.c mem_impl.h mem.h
	gcc -Wall -g -std=c11 -c getmem.c

get_mem_stats.o: get_mem_stats.c mem_impl.h mem.h
	gcc -Wall -g -std=c11 -c get_mem_stats.c

mem_utils.o: mem_utils.c mem_impl.h mem.h
	gcc -Wall -g -std=c11 -c mem_utils.c

print_heap.o: print_heap.c mem_impl.h mem.h
	gcc -Wall -g -std=c11 -c print_heap.c

bench.o: bench.c mem.h
	gcc -Wall -g -std=c11 -c bench.c

quick_test.o: quick_test.c mem_impl.h mem.h
	gcc -Wall -g -std=c11 -c quick_test.c

