#include "board.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define bitscan(x) __builtin_ctz(x)

// Maps moves available from each space (indexed by space number -- see board.h).
const bitboard MOVES[20] = {
	0b000000000000000001110,
	0b000000000000000110101,
	0b000000000000000011011,
	0b000000000010000001101,
	0b000001000100100001110,
	0b000000000000111000010,
	0b000000000001010100000,
	0b000000000001101100000,
	0b000001000101010110000,
	0b000010000000111000000,
	0b000000011100000001000,
	0b000001101010100010000,
	0b000000110110000000000,
	0b000000101010000000000,
	0b001000011100000000000,
	0b001110000100100010000,
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

// Initial positions
const bitboard START_POSITIONS[2] = {
	0b000000000000000011111,
	0b011111000000000000000
};

const char *SPACE_NAMES[20] = {
	"A1",
	"A2",
	"A3",
	"A4",
	"A5",
	"B1",
	"B2",
	"B3",
	"B4",
	"B5",
	"C1",
	"C2",
	"C3",
	"C4",
	"C5",
	"D1",
	"D2",
	"D3",
	"D4",
	"D5"
};

struct Board *Board_create(){
	struct Board *board = malloc(sizeof(struct Board));
	assert(board != NULL);

	board->bits[RED] = START_POSITIONS[RED];
	board->bits[BLU] = START_POSITIONS[BLU];

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

			// Check for empty start quad, for signal pegs
			if((moves[count] & START_POSITIONS[board->turn]) == 0){
				moves[count] |= 1 << 20;
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

void moveQBN(bitboard start, bitboard end, char *out){
	bitboard movedSpaces = start ^ end;
	int s1 = bitscan(movedSpaces);
	int s2 = bitscan(movedSpaces >> (s1 + 1)) + s1 + 1;
	if(((1 << s1) & end) > 0){
		int t = s1;
		s1 = s2;
		s2 = t;
	}
	strncpy(out, SPACE_NAMES[s1], 2);
	out[2] = '-';
	strncpy(out+3, SPACE_NAMES[s2], 2);
	out[5] = '\0';
}
