#include "board.h"

#include <stdio.h>

int main(){
	struct Board *board = Board_create();
	bitboard moves[MAX_MOVES];
	Board_moves(board, moves);
	Board_move(board, moves[0]);
	Board_destroy(board);
	return 0;
}
