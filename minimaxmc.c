#include "board.h"
#include "minimax.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define randint(x) rand() % x

const int MAX_DEPTH = 12;
const int SIM_DEPTH = 4;

const int EVAL_SIMS = 15;
const int MAX_SIM_DEPTH = 100;
const int MAX_SAFE_MOVE_TRIES = 500;

uint64_t nodesSearched = 0;
uint64_t leaves = 0; // non-terminal leaves
uint64_t prunes = 0;

uint64_t simulations = 0;
uint64_t simulationMoves = 0;
uint64_t depthOuts = 0;
uint64_t safeMoveGiveUps = 0;

int simulate(struct Board *board){
	simulations++;
	simulationMoves++;

	enum Color turn = board->turn;	

	bitboard moves[MAX_MOVES];
	int depth = 0;
	struct Board *clone = Board_create();
	while(depth < MAX_SIM_DEPTH){
		int count = Board_moves(board, moves, true);
		if(count < 0){
			// We've found a win
			Board_destroy(clone);
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

		simulationMoves++;
		depth++;
	}
	Board_destroy(clone);

	// Depth out; consider it a draw
	depthOuts++;
	return 0;
}

double search(const struct Board *board, double alpha, double beta, int depth){
	nodesSearched++;

	if(depth == MAX_DEPTH){
		return 0.0;
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

	if(depth == SIM_DEPTH && bestScore == 0.0){
		leaves++;
		double score = 0;
		struct Board *clone = Board_create();
		for(int i = 0; i < EVAL_SIMS; i++){
			*clone = *board;
			score += simulate(clone);
		};
		Board_destroy(clone);
		score /= EVAL_SIMS;
		return score;
	}


	return bestScore;
}

bitboard minimax_think(const struct Board *board){
	time_t start = time(NULL);
	srand(start);

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
	fprintf(stderr, "moves:\t%d\n", count);
	fprintf(stderr, "nodes:\t%luK\n", nodesSearched/1000);
	fprintf(stderr, "prunes:\t%lu\n", prunes);
	fprintf(stderr, "simulations:\t%luK\n", simulations/1000);
	fprintf(stderr, "sim moves:\t%luK\n", simulationMoves/1000);
	fprintf(stderr, "moves/sim:\t%f\n", (float)simulationMoves/simulations);
	fprintf(stderr, "%% depth outs:\t%f%%\n", (float)depthOuts/simulations*100);
	fprintf(stderr, "%% give ups:\t%f%%\n", (float)safeMoveGiveUps/simulationMoves*100);
	fprintf(stderr, "time:\t%lds\n", thinkTime);
	if(thinkTime > 0){
		fprintf(stderr, "speed:\t%ldK nodes/second\n", nodesSearched/thinkTime/1000);
	}

	return bestMove;
}
