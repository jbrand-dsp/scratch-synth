CC = clang
CFLAGS = -std=c99 -Wall
SRCS = main.c wav_writer.c
LDFLAGS = -L/opt/homebrew/lib -I/opt/homebrew/include -lraylib -lm -framework CoreMIDI -framework CoreFoundation

build: clean main.c
	$(CC) $(CFLAGS) main.c wav_writer.c -o synth.o $(LDFLAGS)

clean:
	rm -f *.o synth

format:
	/opt/homebrew/opt/llvm/bin/clang-tidy *.c *.h -- -std=c99 -I/opt/homebrew/include
	clang-format -i *.c *.h