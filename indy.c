#include "board.h"

#include <stdio.h>

int main(){
	struct Board *board = Board_create();
	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);
	Board_move(board, moves[0]);
	printf("%d\n", count);
	Board_destroy(board);
	return 0;
}
