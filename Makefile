CFLAGS = -g -Wall

PROGS = set74880 reset

all: $(PROGS)

set74880: set74880.o
	$(CC) $(CFLAGS) -o set74880 set74880.o

reset: reset.o
	$(CC) $(CFLAGS) -o reset reset.o

clean:
	rm -f *~ *.o $(PROGS)
