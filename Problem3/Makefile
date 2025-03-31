CC = gcc
CFLAGS = -Wall -pthread
TARGETS = chat_client1 chat_client2

all: $(TARGETS)

chat_client1: chat_client1.c
	$(CC) $(CFLAGS) -o chat_client1 $<

chat_client2: chat_client2.c
	$(CC) $(CFLAGS) -o chat_client2 $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean