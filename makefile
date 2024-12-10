# Compiler
CC = gcc

CFLAGS = -I/usr/local/Cellar/cjson/1.7.18/include/cjson

LDFLAGS = -L/usr/local/Cellar/cjson/1.7.18/lib -lcjson -lsqlite3

SRC = main.c database.c

OBJ = $(SRC:.c=.o)

TARGET = bankAPP

all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)
