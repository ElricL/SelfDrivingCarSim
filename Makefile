all: carsim

CFILES=$(wildcard *.c)
ObjFILES=$(CFILES:.c=.o)
HFILES=$(wildcard *.h)
CFLAGS=-std=gnu99 -Wall -g


carsim: $(ObjFILES)
	$(CC) $(CFLAGS) -Wall -pthread -o $@ $^

%.o: %.c $(HFILES)
	gcc $(CFLAGS) -Wall -pthread -o $@ -c $<

clean:
	rm -f *.o