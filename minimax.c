#include "board.h"
#include "minimax.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

const int DEPTH = 8;

int nodesSearched = 0;

double search(const struct Board *board, int depth){
	nodesSearched++;

	if(depth == DEPTH){
		return 0.0;
	}

	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);
	if(count < 0){
		return INFINITY;
	}

	double bestScore = -INFINITY;
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
	time_t start = time(NULL);

	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);

	bitboard bestMove = moves[0];
	double bestScore = -INFINITY;
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

	time_t end = time(NULL);
	time_t thinkTime = end - start;

	fprintf(stderr, "score:\t%lf\n", bestScore);
	fprintf(stderr, "nodes:\t%dK\n", nodesSearched/1000);
	fprintf(stderr, "time:\t%lds\n", thinkTime);
	if(thinkTime > 0){
		fprintf(stderr, "speed:\t%ldK nodes/second\n", nodesSearched/thinkTime/1000);
	}

	return bestMove;
}
