CFLAGS=-Wall -g

all: indy

indy: board.o minimax.o montecarlo.o

clean:
	rm indy
	rm *.o

fresh: clean all
