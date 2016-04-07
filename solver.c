#include "board.h"
#include "solver.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define randint(x) rand() % x

const int MAX_SIM_DEPTH = 100;
const int MAX_SAFE_MOVE_TRIES = 500;

struct Node{
	struct Board board;

	struct Node *children[MAX_MOVES];
	bitboard moves[MAX_MOVES];
	int count;

	int visits;
	double value;
};

struct Node *Node_create(const struct Board *board){
	struct Node *node = malloc(sizeof(struct Node));
	assert(node != NULL);

	node->board = *board;
	node->count = Board_moves(board, node->moves, true);
	node->visits = 0;
	node->value = 0.0;

	return node;
}

void Node_makeChildren(struct Node *node){
	struct Board board;
	for(int i = 0; i < node->count; i++){
		board = node->board;
		Board_move(&board, node->moves[i]);
		node->children[i] = Node_create(&board);
	}
}

struct Node *selectChild(const struct Node *node){
	double bestUCT = -INFINITY;
	struct Node *bestChild = node->children[0];
	for(int i = 0; i < node->count; i++){
		struct Node *child = node->children[i];
		double score = child->value/child->visits;
		double uct = score + sqrt(1.0*log(node->visits) / child->visits);
		if(uct > bestUCT){
			bestUCT = uct;
			bestChild = child;
		}
	}
	return bestChild;
}

int simulate(struct Board *board){
	enum Color turn = board->turn;	

	bitboard moves[MAX_MOVES];
	struct Board *clone = Board_create();
	int depth = 0;
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
		Board_move(board, moves[move]);

		depth++;
	}
	Board_destroy(clone);

	// Depth out; consider it a draw
	return 0;
}

double search(struct Node *node){
	// if we  have a win, score infinity
	if(node->count == -1){
		node->value = INFINITY;
		return INFINITY;
	}
	
	// if this is our first visit to the node (apart from the initial
	// simulation), create nodes for its children
	if(node->visits == 1){
		Node_makeChildren(node);
	}

	// add another visit
	node->visits++;

	// select a child for further exploration
	struct Node* bestChild = selectChild(node);

	double result;
	if(bestChild->value == -INFINITY || bestChild->value == INFINITY){
		result = -bestChild->value;
	}else{
		// if this is this child's first visit, run a simulation from it
		if(bestChild->visits < 1){
			struct Board board;
			board = bestChild->board;
			result = -simulate(&board);
			bestChild->visits++;
		}else{
			result = -search(bestChild);
		}
	}

	if(result == INFINITY){
		node->value = INFINITY;
		return INFINITY;
	}else if(result == -INFINITY){
		for(int i = 0; i < node->count; i++){
			if(node->children[i]->value != result){
				result = -1;
				break;
			}
		}
		
		if(result == -INFINITY){
			node->value = -INFINITY;
			return -INFINITY;
		}
	}
	
	node->value += result;
	return result;
}

bitboard solver_think(const struct Board *board){
	time_t start = time(NULL);

	struct Node *node = Node_create(board);

	search(node);

	double bestScore = -INFINITY;
	double bestMove = node->moves[0];
	for(int i = 0; i < node->count; i++){
		struct Node *child = node->children[i];;
		double score = child->value/child->visits + 1/sqrt(child->visits);
		if(score > bestScore){
			bestScore = score;
			bestMove = node->moves[i];
		}
	}

	return bestMove;
}
