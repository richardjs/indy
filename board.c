#include "board.h"

#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

#define bitscan(x) __builtin_ctz(x)

const int MAX_MOVES = 5;
const bitboard MOVES[20] = {
	0b000000000000000001110,
	0b000000000000000110101,
	0b000000000000000011011,
	0b000000000010000001101,
	0b000000000100100001110,
	0b000000000000111000010,
	0b000000000001010100000,
	0b000000000001101100000,
	0b000001000001010110000,
	0b000010000000111000000,
	0b000000011100000001000,
	0b000001101010000010000,
	0b000000110110000000000,
	0b000000101010000000000,
	0b001000011100000000000,
	0b001110000100100000000,
	0b010101000001000000000,
	0b011011000000000000000,
	0b010101100000000000000,
	0b001110000000000000000
};

struct Board *Board_create(){
	struct Board *board = malloc(sizeof(struct Board));
	assert(board != NULL);

	board->bits[RED] = 0b100000000000000011111;
	board->bits[BLU] = 0b111111000000000000000;

	return board;
}

void Board_destroy(struct Board *board){
	free(board);
	return;
}

int Board_moves(const struct Board *board, bitboard moves[MAX_MOVES]){
	int count = 0;
	int space = 0;
	for(int i = 0; i < 5; i++){
		space = bitscan(board->bits[board->turn] >> space) + space;
		bitboard openMoves = MOVES[space] & ~(board->bits[board->turn] | board->bits[!board->turn]);
		
		int moveSpace = 0;
		while(openMoves >> moveSpace){
			moveSpace = bitscan(openMoves >> moveSpace) + moveSpace;
			moves[count] = board->bits[board->turn];
			moves[count] &= ~(1 << space);
			moves[count] |= 1 << moveSpace;
			count++;
			moveSpace++;
		}

		space++;
	}
	return count;
}
