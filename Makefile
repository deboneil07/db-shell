#defining the compiler

CC = gcc

CFLAGS = -g

TARGET = my_shell

SRC = my_shell.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o  $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
