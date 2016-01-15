CFLAGS=-Wall -g

all: indy

indy: board.o

clean:
	rm indy
	rm *.o
