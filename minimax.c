#include "board.h"
#include "minimax.h"

#include <math.h>
#include <stdio.h>

const int DEPTH = 8;

double search(const struct Board *board, int depth){
	if(depth == DEPTH){
		return 0.0;
	}

	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);
	if(count <= 0){
		return 1.0;
	}

	double bestScore = -1.0;
	for(int i = 0; i < count; i++){
		struct Board *clone = Board_create();
		*clone = *board;
		Board_move(clone, moves[i]);
		double score = -search(clone, depth + 1);
		if(score > bestScore){
			bestScore = score;
		}
		Board_destroy(clone);
	}

	return bestScore;
}

bitboard minimax_think(const struct Board *board){
	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);
	if(count <= 0){
		return moves[0];
	}

	bitboard bestMove;
	double bestScore = -10.0;
	for(int i = 0; i < count; i++){
		struct Board *clone = Board_create();
		*clone = *board;
		Board_move(clone, moves[i]);
		double score = -search(clone, 1);
		if(score > bestScore){
			bestMove = moves[i];
			bestScore = score;
		}
	}

	fprintf(stderr, "score: %lf\n", bestScore);

	return bestMove;
}
