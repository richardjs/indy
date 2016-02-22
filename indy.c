#include "board.h"
#include "minimax.h"
#include "montecarlo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	fprintf(stderr, "---\ninput:\t%s\n", qbn);

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

	bitboard moves[MAX_MOVES];
	bitboard move = 0;
	int count = Board_moves(board, moves, true);
	if(count == 0){
		fprintf(stderr, "no moves\n");
		return 0;
	}
	if(count < 0){
		fprintf(stderr, "taking winning move\n");
		move = moves[0];
	}

	// Seed random and run Monte Carlo algorithm
	//srand(time(NULL));
	//bitboard move = montecarlo_think(board);

	// Run minimax search
	if(!move){
		fprintf(stderr, "running minimax\n");
		move = minimax_think(board);
	}

	// Print move as QMN
	char qmn[6];
	moveQBN(board->bits[board->turn], move, qmn);
	fprintf(stderr, "move:\t%s\n", qmn);
	printf("%s\n", qmn);

	Board_destroy(board);
	return 0;
}
