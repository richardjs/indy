#ifndef BOARD_H
#define BOARD_H

#include "stdint.h"

/*
Board bits layout (each color gets its own board):

 0   1  5   6
   2      7
 3   4  8   9
10  11 15  16
  12     17
13  14 18  19

20th bit represents the color's signal peg, with a 0 indicating the
initial quad is not available, and a 1 indicating it is available.
*/

typedef uint32_t bitboard;

enum Color{RED=0, BLU};

extern const int MAX_MOVES;

struct Board{
	bitboard bits[2];
	enum Color turn;
};

struct Board *Board_create();
void Board_destroy();

int Board_moves(const struct Board *board, bitboard moves[]);
void Board_move(struct Board *board, bitboard move);

#endif
