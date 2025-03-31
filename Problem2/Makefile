CC = gcc
CFLAGS = -Wall -O2
LIBS = -lpthread

all: sum_serial sum_multi_thread

sum_serial: sum_serial.c
	$(CC) $(CFLAGS) -o sum_serial sum_serial.c

sum_multi_thread: sum_multi_thread.c
	$(CC) $(CFLAGS) -o sum_multi_thread sum_multi_thread.c $(LIBS)

clean:
	rm -f sum_serial sum_multi_thread