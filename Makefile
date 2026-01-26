CC = clang
CFLAGS = -std=c99 -Wall

build: clean main.c
	$(CC) $(CFLAGS) main.c -o synth.o

clean:
	rm -f *.o synth