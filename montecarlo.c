#include "board.h"
#include "montecarlo.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define randint(x) rand() % x

const int SIMS_PER_MOVE = 10000;
const int MAX_SIM_DEPTH = 100;
const int MAX_SAFE_MOVE_TRIES = 500;

int simulations = 0;
int totalMoves = 0;
int depthOuts = 0;
int safeMoveGiveUps = 0;

int simulate(struct Board *board){
	simulations++;
	totalMoves++;

	// board's turn is opposite of thinking player, since the move
	// to be considered has already been made
	enum Color turn = !board->turn;	

	bitboard moves[MAX_MOVES];
	int depth = 0;
	struct Board *clone = Board_create();
	while(depth < MAX_SIM_DEPTH){
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

		int tries = 0;
		int move = randint(count);
		while(tries < MAX_SAFE_MOVE_TRIES){
			*clone = *board;
			Board_move(clone, moves[move]);
			if(Board_moves(clone, NULL, true) >= 0){
				break;
			}
			move = randint(count);
			tries++;
		}
		if(tries == MAX_SAFE_MOVE_TRIES){
			safeMoveGiveUps++;
		}
		Board_move(board, moves[move]);

		totalMoves++;
		depth++;
	}
	Board_destroy(clone);

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
	fprintf(stderr, "safe give ups:\t%d\n", safeMoveGiveUps);
	fprintf(stderr, "%% give ups:\t%f%%\n", (float)safeMoveGiveUps/totalMoves*100);
	fprintf(stderr, "time:\t\t%lds\n", thinkTime);
	if(thinkTime > 0){
		fprintf(stderr, "sims/second:\t%ldK\n", simulations/thinkTime/1000);
		fprintf(stderr, "moves/second:\t%ldK\n", totalMoves/thinkTime/1000);
	}

	return bestMove;
}
