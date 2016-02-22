#include "board.h"
#include "montecarlo.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define randint(x) rand() % x

const int SIMS_PER_MOVE = 1000000;

int simulations = 0;
int totalMoves = 0;
int depthOuts = 0;

int simulate(struct Board *board){
	simulations++;
	totalMoves++;

	// board's turn is opposite of thinking player, since the move
	// to be considered has already been made
	enum Color turn = !board->turn;	

	bitboard moves[MAX_MOVES];
	int depth = 0;
	while(depth < 100){
		int count = Board_moves(board, moves, true);
		if(count < 0){
			// We've found a win
			if(board->turn == turn){
				return 1;
			}
			return -1;
		}
		if(count == 0){
			board->turn = !board->turn;
			depth++;
			continue;
		}
		Board_move(board, moves[randint(count)]);
		totalMoves++;
		depth++;
	}
	// Depth out; consider it a draw
	depthOuts++;
	return 0;
}

bitboard montecarlo_think(const struct Board *board){
	time_t start = time(NULL);

	bitboard moves[MAX_MOVES];
	int count = Board_moves(board, moves, true);

	fprintf(stderr, "moves:\t\t%d\n", count);

	int scores[MAX_MOVES] = {0};
	struct Board *clone = Board_create();
	for(int i = 0; i < count; i++){
		for(int j = 0; j < SIMS_PER_MOVE; j++){
			*clone = *board;
			Board_move(clone, moves[i]);
			scores[i] += simulate(clone);
		}
	}

	float bestScore = -INFINITY;
	bitboard bestMove = moves[0];
	for(int i = 0; i < count; i++){
		float score = (float)scores[i] / SIMS_PER_MOVE;
		if(score > bestScore){
			bestScore = score;
			bestMove = moves[i];
		}
	}

	time_t end = time(NULL);
	time_t thinkTime = end - start;

	fprintf(stderr, "score:\t\t%lf\n", bestScore);
	fprintf(stderr, "simulations:\t%dK\n", simulations/1000);
	fprintf(stderr, "sim moves:\t%dK\n", totalMoves/1000);
	fprintf(stderr, "moves/sim:\t%f\n", (float)totalMoves/simulations);
	fprintf(stderr, "depth outs:\t%dK\n", depthOuts/1000);
	fprintf(stderr, "%% depth outs:\t%f%%\n", (float)depthOuts/simulations*100);
	fprintf(stderr, "time:\t\t%lds\n", thinkTime);
	fprintf(stderr, "sims/second:\t%ldK\n", simulations/thinkTime/1000);
	fprintf(stderr, "moves/second:\t%ldK\n", totalMoves/thinkTime/1000);

	return bestMove;
}
