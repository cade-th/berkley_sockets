CC = clang
CFLAGS = -Wall -std=c99 -O2
LDFLAGS = 

SRC = $(wildcard src/*.c)
OUT = b_sockets

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)

