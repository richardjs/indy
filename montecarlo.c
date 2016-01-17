#include "montecarlo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define randint(x) rand() % x

const int SIMS_PER_MOVE = 10000;

bool simulate(struct Board *board){
	enum Color turn = board->turn;	
	bitboard moves[MAX_MOVES];
	int depth = 0;
	while(depth < 100){
		int count = Board_moves(board, moves, true);
		if(count == -1){
			return board->turn == turn;
		}
		Board_move(board, moves[randint(count)]);
		depth++;
	}
	// depth out
	return 0;
}

bitboard montecarlo_think(const struct Board *board){
	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);
	if(count == -1){
		return moves[0];
	}

	int wins[MAX_MOVES];
	struct Board *clone = Board_create();
	for(int i = 0; i < count; i++){
		for(int j = 0; j < SIMS_PER_MOVE; j++){
			*clone = *board;
			Board_move(clone, moves[i]);
			wins[i] += simulate(clone);
		}
	}

	float bestScore;
	bitboard bestMove;
	for(int i = 0; i < count; i++){
		float score = wins[i] / SIMS_PER_MOVE;
		if(score > bestScore){
			bestScore = score;
			bestMove = moves[i];
		}
	}
	return bestMove;
}
