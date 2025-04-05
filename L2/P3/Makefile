CC = gcc
CFLAGS = -Wall -pthread
TARGETS = msgrcv msgsnd

all: $(TARGETS)

msgrcv: msgrcv.c
	$(CC) $(CFLAGS) -o msgrcv $<

msgsnd: msgsnd.c
	$(CC) $(CFLAGS) -o msgsnd $<

clean:
	rm -f $(TARGETS)
