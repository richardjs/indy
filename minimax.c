#include "board.h"
#include "minimax.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

const int DEPTH = 10;

uint64_t nodesSearched = 0;
uint64_t leaves = 0; // non-terminal leaves
uint64_t prunes = 0;

double MOVE_VALUE = 1;
double QUAD_VALUE = 100;
double PIN_VALUE = 1000;
double UNOPPOSED_VALUE = -500;

double evaluate(const struct Board *board){
	double score = 0;
	if(board->turn != 0 && (board->bits[board->turn] & 0b000000000000000011111) > 0){
		score += QUAD_VALUE;
	}
	if((board->bits[board->turn] & 0b000000000000010000000) > 0){
		score += QUAD_VALUE;
	}
	if((board->bits[board->turn] & 0b000000001000000000000) > 0){
		score += QUAD_VALUE;
	}
	if(board->turn != 1 && (board->bits[board->turn] & 0b011111000000000000000) > 0){
		score += QUAD_VALUE;
	}

	if((board->bits[board->turn] & 0b000000000001111100000) == 0 &&
			(board->bits[!board->turn] & 0b000000000001111100000) > 0){
		score += UNOPPOSED_VALUE;
	}
	if((board->bits[board->turn] & 0b000000111110000000000) == 0 &&
			(board->bits[!board->turn] & 0b000000111110000000000) > 0){
		score += UNOPPOSED_VALUE;
	}

	if((board->bits[board->turn] & 0b100000000000000000000) > 0){
		score += PIN_VALUE;
	}



	int moves = Board_moves(board, NULL, false);
	score += moves*MOVE_VALUE;

	return score;
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
		fprintf(stderr, ".");
		fflush(stderr);
	}
	fprintf(stderr, "\n");

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
