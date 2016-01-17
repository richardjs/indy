#include "board.h"
#include "montecarlo.h"

#include <stdio.h>

int main(){
	struct Board *board = Board_create();

	bitboard move = montecarlo_think(board);
	printf("%d\n", move);

	Board_destroy(board);
	return 0;
}
