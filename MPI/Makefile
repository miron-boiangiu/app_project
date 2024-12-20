CC = mpicc
CFLAGS = -I. -g -pg -fno-inline -pthread
DEPS = image_editor.h cbmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o image_editor.o cbmp.o
	$(CC) $(CFLAGS) -o main main.o image_editor.o cbmp.o -lm


test_small:
	mpirun -np 4 ./main images/small.bmp output.bmp 1

test_medium:
	mpirun -np 4 ./main images/medium.bmp output.bmp 10

test_big:
	mpirun -np 4 ./main images/big.bmp output.bmp 20

clean:
	rm *.o main