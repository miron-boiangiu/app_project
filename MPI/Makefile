CC = mpicc
CFLAGS = -I. -g -pg -fno-inline -fno-optimize-sibling-calls
DEPS = image_editor_MPI.h cbmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o image_editor_MPI.o cbmp.o
	$(CC) $(CFLAGS) -o main main.o image_editor_MPI.o cbmp.o


test_small:
	./main images/small.bmp output_small.bmp 1

test_medium:
	./main images/medium.bmp output_medium.bmp 5

test_big:
	./main images/big.bmp output_big.bmp 10

clean:
	rm *.o main