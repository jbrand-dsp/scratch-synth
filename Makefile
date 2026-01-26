CC = clang
CFLAGS = -std=c99 -Wall
SRCS = main.c wav_writer.c

build: clean main.c
	$(CC) $(CFLAGS) main.c wav_writer.c -o synth.o

clean:
	rm -f *.o synth