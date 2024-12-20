CC = gcc
CFLAGS = -I. -g -pthread
DEPS = image_editor.h cbmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o image_editor.o cbmp.o
	$(CC) $(CFLAGS) -o main main.o image_editor.o cbmp.o -lm


test_small:
	./main images/small.bmp output_small.bmp 1 4

test_medium:
	./main images/medium.bmp output_medium.bmp 5 4

test_big:
	./main images/big.bmp output_big.bmp 10 4

clean:
	rm *.o main