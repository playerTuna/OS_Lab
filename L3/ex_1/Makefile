CC = gcc
CFLAGS = -std=c11 -Wall -pthread
TARGET = seqlock
SRC = seqlock.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
