#include "board.h"
#include "montecarlo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]){
	// Validate input
	if(argc != 2){
		puts("usage: indy <qbn>");
		return 1;
	}
	char* qbn = argv[1];
	if(strlen(qbn) != 23){
		puts("invalid qbn");
		return 1;
	}

	// Build board from QBN
	struct Board *board = Board_create();
	board->bits[RED] = 0;
	board->bits[BLU] = 0;
	for(int i = 0; i < 20; i++){
		switch(qbn[i]){
			case '1':
				board->bits[RED] |= 1 << i;
				break;
			case '2':
				board->bits[BLU] |= 1 << i;
				break;
		}
	}
	if(qbn[20] == '1'){
		board->bits[RED] |= 1 << 20;
	}
	if(qbn[21] == '1'){
		board->bits[BLU] |= 1 << 20;
	}
	if(qbn[22] == '2'){
		board->turn = BLU;
	}

	// Seed random and run Monte Carlo algorithm
	srand(time(NULL));
	bitboard move = montecarlo_think(board);

	// Print move as QMN
	char qmn[6];
	moveQBN(board->bits[board->turn], move, qmn);
	printf("%s\n", qmn);

	Board_destroy(board);
	return 0;
}
