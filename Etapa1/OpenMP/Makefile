CC=gcc
CFLAGS=-I. -g
DEPS = image_editor.h cbmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -fopenmp

main: main.o image_editor.o cbmp.o
	$(CC) -o main main.o image_editor.o cbmp.o -fopenmp

test_small:
	./main images/small.bmp output_small.bmp 1

test_medium:
	./main images/medium.bmp output_medium.bmp 5

test_big:
	./main images/big.bmp output_big.bmp 10

clean:
	rm *.o main