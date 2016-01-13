#ifndef BOARD_H
#define BOARD_H

#include "stdint.h"

typedef uint32_t bitboard;

enum Color{RED=0, BLUE};

struct Board{
	bitboard bits[2];
	enum Color turn;
};

struct Board *Board_create();
void Board_destroy();

#endif
