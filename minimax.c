#include "board.h"
#include "minimax.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

const int DEPTH = 9;

uint64_t nodesSearched = 0;
uint64_t leaves = 0; // non-terminal leaves
uint64_t prunes = 0;

double evaluate(const struct Board *board){
}

double search(const struct Board *board, double alpha, double beta, int depth){
	nodesSearched++;

	if(depth == DEPTH){
		leaves++;
		return evaluate(board);
	}

	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);
	if(count < 0){
		return INFINITY;
	}

	double bestScore = -INFINITY;
	struct Board *clone = Board_create();
	for(int i = 0; i < count; i++){
		*clone = *board;
		Board_move(clone, moves[i]);
		double score = -search(clone, -beta, -alpha, depth + 1);
		if(score > bestScore){
			bestScore = score;
		}

		if(score > alpha){
			alpha = score;
		}
		if(alpha >= beta){
			prunes++;
			break;
		}
	}
	Board_destroy(clone);

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
		double score = -search(clone, -INFINITY, INFINITY, 1);
		if(score > bestScore){
			bestMove = moves[i];
			bestScore = score;
		}
	}

	time_t end = time(NULL);
	time_t thinkTime = end - start;

	fprintf(stderr, "score:\t%lf\n", bestScore);
	fprintf(stderr, "nodes:\t%lu\n", nodesSearched/1000);
	fprintf(stderr, "leaves:\t%lu\n", leaves/1000);
	fprintf(stderr, "prunes:\t%lu\n", prunes);
	fprintf(stderr, "time:\t%lds\n", thinkTime);
	if(thinkTime > 0){
		fprintf(stderr, "speed:\t%ldK nodes/second\n", nodesSearched/thinkTime/1000);
	}

	return bestMove;
}
