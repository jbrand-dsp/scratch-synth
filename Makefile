CC = clang
CFLAGS = -std=c99 -Wall
SRCS = main.c wav_writer.c

build: clean main.c
	$(CC) $(CFLAGS) main.c wav_writer.c -o synth.o

clean:
	rm -f *.o synth

format:
	/opt/homebrew/opt/llvm/bin/clang-tidy *.c *.h -- -std=c99
	clang-format -i *.c *.h