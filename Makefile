SOURCES=src/main.c
EXEC=triangles

CC=gcc
CFLAGS=-ggdb -O0

default: $(EXEC)

$(EXEC): $(SOURCES)
	$(CC) $(CFLAGS) -I./inc -o $@ $^

clean:
	rm -rf *.o $(EXEC)

.PHONY: default clean
