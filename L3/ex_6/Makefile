CC = gcc
CFLAGS = -Wall -pthread
TARGET = main
SRC = main.c async_resource.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
