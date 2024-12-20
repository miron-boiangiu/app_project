CC = mpicc
CFLAGS = -I. -g -fopenmp
DEPS = image_editor.h cbmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -fopenmp

main: main.o image_editor.o cbmp.o
	$(CC) $(CFLAGS) -o main main.o image_editor.o cbmp.o -lm -fopenmp


test_small:
	mpirun -np 4 ./main images/small.bmp output.bmp 1

test_medium:
	mpirun -np 4 ./main images/medium.bmp output.bmp 5

test_big:
	mpirun -np 4 ./main images/big.bmp output.bmp 10

clean:
	rm *.o main