CFLAGS=-Wall -g -O3 -lm

all: indy

#indy: board.o minimaxmc.o
#indy: board.o montecarlo.o
#indy: board.o solver.o
indy: board.o minimax.o

clean:
	rm indy
	rm *.o

fresh: clean all
