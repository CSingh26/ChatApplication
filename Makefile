CC=gcc
CFLAGS=-I include -Wall -g
DEPS = include/common.h include/network.h
OBJ = src/server.o src/client.o src/network.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server client

server: src/server.o src/network.o
	mkdir -p bin
	$(CC) -o bin/server $^ $(CFLAGS)

client: src/client.o src/network.o
	mkdir -p bin
	$(CC) -o bin/client $^ $(CFLAGS)

clean:
	rm -f src/*.o $(OUTPUT_DIR)/server $(OUTPUT_DIR)/*.dSYM
