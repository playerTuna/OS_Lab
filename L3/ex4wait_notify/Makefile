CC = gcc
CFLAGS = -Wall -pthread
TARGET = wait_notify

all: $(TARGET) run

$(TARGET): wait_notify.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) *.o
run:
	./$(TARGET)

.PHONY: all clean