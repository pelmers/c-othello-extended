#ifndef HEADER
#define HEADER

#include <memory.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//screen width for the progress bar
#define SCR_WIDTH 80

//define a few constants used on the board
#define EMPTY 0
#define WHITE 1
#define BLACK -1
#define BORDER 2
// the actual max and min are far lower, but it's good to be safe
#define MAX_SCORE 9999
#define MIN_SCORE -9999

// choices for source selection
#define HUMAN 0
#define RANDOM 1
#define SHALLOW 2
#define MINIMAX 3
#define ALPHABETA 4

// END_DEPTH controls when the AI begins to search all the way to end
// S_DEPTH is all other times
#define S_DEPTH 3
#define END_DEPTH 9

// macros to find maximum and minimum of two integers
#define MAX(a, b) (((a) + (b) + abs((a) - (b))) * 0.5)
#define MIN(a, b) (((a) + (b) - abs((a) - (b))) * 0.5)

// for ever loops!
#define ever (;;)

// board.c
void to_flip(int *board, int move, int side, int *flips);
void make_move(int *board, int move, int side, int *flips);
char legal_move(int *board, int move, int side, int *flips);
char test_possible_moves(int *board, int side, int *flips);
char test_end(int *board, int unplayed);
int find_score(int *board, int side);
inline void reset_flips(int *flips);
void empty_board(int *board);
void default_board(int *board);
void copy_board(int *oldboard, int *newboard);
char play_turn(int *board, int *side, int *unplayed, int show, 
        int black_source, int white_soure, int *flips);
// ai.c
int evaluate_board(int *board, int side, int unplayed);
int get_human_move(int *board, int side);
int get_random_move(int *board, int side);
int get_shallow_move(int *board, int side);
int maximize(int *board, int side, int unplayed, int ply);
int minimize(int *board, int side, int unplayed, int ply);
int get_minimax_move(int *board, int side, int unplayed, int ply);
int ab_maximize(int *board, int side, int unplayed, int ply, int a, int b);
int ab_minimize(int *board, int side, int unplayed, int ply, int a, int b);
int get_alphabeta_move(int *board, int side, int unplayed, int ply);
// main.c
int get_source(int side);
int get_simulate_number();
int get_randomize();
void print_board(int *board, int side);
void print_victor(int *board);
int get_move(int *board, int side, int source, int unplayed);
int main();
void progress_bar(int width, int percent);

extern const int directions[8];
extern const int weights[100];


#endif
