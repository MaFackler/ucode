CC=g++
CFLAGS=-Wall -Wextra

all: ucode

ucode: src/main.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm ucode
