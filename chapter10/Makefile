C_SOURCES = $(shell find assignment/ -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

CC = gcc
C_FLAGS = -Wall -I include/ -L lib/

build: $(C_OBJECTS)

.c.o:
	$(CC) $(C_FLAGS) $< -o $@ -lcsapp64 -lpthread