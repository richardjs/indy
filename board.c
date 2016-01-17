#include "board.h"

#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

#define bitscan(x) __builtin_ctz(x)

const int MAX_MOVES = 25;

// Maps moves available from each space (indexed by space number -- see board.h).
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

// List of board states that trigger a win for either side.
// These don't invole the signal peg (the 21st bit).
const bitboard ALL_WINS[16] = {
	0b00000000000011100000,
	0b00000000001011000000,
	0b00000000001110000000,
	0b00000000000110100000,
	0b00000000001001100000,
	0b00000000001101000000,
	0b00000000001100100000,
	0b00000000000101100000,
	0b00000001110000000000,
	0b00000101100000000000,
	0b00000111000000000000,
	0b00000011010000000000,
	0b00000100110000000000,
	0b00000110100000000000,
	0b00000110010000000000,
	0b00000010110000000000
};

// List of board states that trigger a win for only one color.
// These involve the signal peg.
const bitboard COLOR_WINS[2][16] = {{
	0b000111000000000000000,
	0b010110000000000000000,
	0b011100000000000000000,
	0b001101000000000000000,
	0b010011000000000000000,
	0b011010000000000000000,
	0b011001000000000000000,
	0b001011000000000000000,
	0b100000000000000000111,
	0b100000000000000010110,
	0b100000000000000011100,
	0b100000000000000001101,
	0b100000000000000010011,
	0b100000000000000011010,
	0b100000000000000011001,
	0b100000000000000001011
},{
	0b100111000000000000000,
	0b110110000000000000000,
	0b111100000000000000000,
	0b101101000000000000000,
	0b110011000000000000000,
	0b111010000000000000000,
	0b111001000000000000000,
	0b101011000000000000000,
	0b000000000000000000111,
	0b000000000000000010110,
	0b000000000000000011100,
	0b000000000000000001101,
	0b000000000000000010011,
	0b000000000000000011010,
	0b000000000000000011001,
	0b000000000000000001011
}};

struct Board *Board_create(){
	struct Board *board = malloc(sizeof(struct Board));
	assert(board != NULL);

	board->bits[RED] = 0b000000000000000011111;
	board->bits[BLU] = 0b011111000000000000000;

	return board;
}

void Board_destroy(struct Board *board){
	free(board);
	return;
}

int Board_moves(const struct Board *board, bitboard moves[MAX_MOVES], bool searchForWin){
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

			if(searchForWin){
				for(int  i = 0; i < 16; i++){
					if((moves[count] & ALL_WINS[i]) == ALL_WINS[i]
							|| (moves[count] & COLOR_WINS[board->turn][i]) == COLOR_WINS[board->turn][i]){
						moves[0] = moves[count];
						return -1;
					}
				}
			}

			count++;
			moveSpace++;
		}

		space++;
	}
	return count;
}

void Board_move(struct Board *board, bitboard move){
	board->bits[board->turn] = move;
	board->turn = !board->turn;
}
