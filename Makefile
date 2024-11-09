CC=gcc
CFLAGS=-I.
DEPS = image_editor.h cbmp.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o image_editor.o cbmp.o
	$(CC) -o main main.o image_editor.o cbmp.o

clean:
	rm *.o main