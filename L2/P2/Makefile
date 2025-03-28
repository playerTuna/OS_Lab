CC = gcc
CFLAGS = -Wall -pthread

all: sum_serial sum_multi_thread

sum_serial: sum_serial.c
	$(CC) $(CFLAGS) -o sum_serial sum_serial.c

sum_multi-thread: sum_multi-thread.c
	$(CC) $(CFLAGS) -o sum_multi_thread sum_multi_thread.c

clean:
	rm -f sum_serial sum_multi_thread
benchmark: all
	@echo "Running benchmark..."
	@N=1000000000; \
	T=10; \
	echo "N = $$N, Threads = $$T"; \
	echo "--------------------------------"; \
	echo "[Serial Version]"; \
	start_time=`date +%s.%N`; \
	./sum_serial $$N > /dev/null; \
	end_time=`date +%s.%N`; \
	serial_time=`echo "$$end_time - $$start_time" | bc`; \
	echo "Serial time: $$serial_time sec"; \
	echo; \
	echo "[Multi-thread Version]"; \
	start_time=`date +%s.%N`; \
	./sum_multi_thread $$T $$N > /dev/null; \
	end_time=`date +%s.%N`; \
	mt_time=`echo "$$end_time - $$start_time" | bc`; \
	echo "Multi_thread time: $$mt_time sec"; \
	echo; \
	speedup=`echo "$$serial_time / $$mt_time" | bc -l`; \
	printf "Speed-up: %.2fx\n" $$speedup

