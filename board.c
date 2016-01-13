#include "board.h"

#include <assert.h>
#include <stdlib.h>

struct Board *Board_create(){
	struct Board *board = malloc(sizeof(struct Board));
	assert(board != NULL);
	return board;
}

void Board_destroy(struct Board *board){
	free(board);
	return;
}
