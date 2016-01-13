#include "board.h"

int main(){
	struct Board *board = Board_create();
	Board_destroy(board);
	return 0;
}
