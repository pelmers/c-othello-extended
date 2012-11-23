#include "c-othello.h"

//right, left, up, down, diagonals
const int directions[8] = {1,-1, 10,-10, 9,-9, 11,-11};

void reset_flips(int *flips) {
    /* Takes a pointer to flips array as input
     * Return nothing, but reset all flips to 0
     */
    memset(flips, 0, sizeof(int)*18);
}

void copy_board(int *oldboard, int *newboard) {
    /* Make the array newboard a copy of the array oldboard
     * Return nothing, but modify newboard
     */
    // again, memcpy from memory.h is faster than loop for copying data
    memcpy(newboard, oldboard, 100*sizeof(int));
}

void to_flip(int *board, int move, int side, int *flips) {
    /* Take move and side as input along with board and flips arrays
     * Examine the move to see what tiles will be flipped
     * Store the positions of flippable tiles in *flips
     */
    // d keeps track of direction being followed
    // n keeps track of index of new_flips[]
    int d, n;
    int next;
    // the most flippable in one direction is 6
    int new_flips[8];
    // i keeps track of index in flips[]
    int i = 0;
    for (d=0; d<8; d++) {
        n = 0;
        next = move + directions[d];
        if (board[next] != -side)
            continue;
        new_flips[0] = next;
        while (board[next] != EMPTY && board[next] != BORDER) {
            next += directions[d];
            if (board[next] == -side)
                new_flips[++n] = next;
            if (board[next] == side) {
                while (n>=0)
                    flips[i++] = new_flips[n--];
                break;
            }
        }
    }
}

void make_move(int *board, int move, int side, int *flips) {
    /* Make a move
     * typically called after legal_move!
     */
    int i;
    // set a tile at this location
    board[move] = side;
    for (i=0; flips[i] != 0; ++i)
        // flip each position in flips
        board[flips[i]] = side;
}

char legal_move(int *board, int move, int side, int *flips) {
    /* Return 0 if the move is not legal
     * Return 1 if legal
     * Update flips to the flips found in the move
     */
    if (board[move] != EMPTY)
        // it has to be empty to place a move there
        return 0;
    reset_flips(flips);
    to_flip(board, move, side, flips);
    if (flips[0] == 0)
        // no flips found, therefore not a valid move
        return 0;
    return 1;
}

char test_possible_moves(int *board, int side, int *flips) {
    /* Return 1 if there is any possible move for side
     */
    int i;
    for (i=11;i<89;++i) {
        if (legal_move(board, i, side, flips) == 1)
            // if any move on the board is legal, there is a possible move
            return 1;
    }
    // out of loop without having returned, no moves found
    return 0;
}

char test_end(int *board, int unplayed) {
    /* Return 1 if the game has ended
     * conditions: two consecutive unplayed turns or no empty spaces left
     */
    int i;
    if (unplayed == 2)
        return 1;
    for (i=11;i<89;++i) {
        if (board[i] == EMPTY)
            // game hasn't ended if there is any empty square
            return 0;
    }
    return 1;
}

int find_score(int *board, int side) {
    /* Return the number of tiles a side has
     */
    int i = 11;
    int n = 0;
    for (i=11;i<89;++i) {
        if (board[i] == side)
            n++;
    }
    return n;
}

int get_move(int *board, int side, int source, int unplayed) {
    /* Call an appropriate function to get a move from specified source
     * Return move as integer
     */
    if (source == HUMAN)
        return get_human_move(board, side);
    if (source == RANDOM)
        return get_random_move(board, side);
    if (source == SHALLOW)
        return get_shallow_move(board, side);
    // find out how many empty squares are left
    int i;
    int count = 0;
    for (i=11;i<89;++i) {
        if (board[i] == EMPTY)
            count++;
    }
    if (source == MINIMAX) {
        if (count >= END_DEPTH)
            // search to S_DEPTH
            return get_minimax_move(board, side, unplayed, S_DEPTH);
        else
            // search to end of the game
            return (side == BLACK)?
                get_minimax_move(board, side, unplayed, END_DEPTH)
                :get_minimax_move(board, side, unplayed, count - 1);
    }
    if (source == ALPHABETA) {
        if (count >= END_DEPTH)
            return get_alphabeta_move(board, side, unplayed, S_DEPTH);
        else
            return (side == BLACK)?
                get_alphabeta_move(board, side, unplayed, END_DEPTH)
                :get_alphabeta_move(board, side, unplayed, count - 1);
    }
    return 0;
}


char play_turn(int *board, int *side, int *unplayed, int show, 
        int black_source, int white_source, int *flips) {
    /* Handle the playing of a turn
     * Modifies side and unplayed if necessary
     * Return 1 if game continues, 0 if ends
     */
    int move;
    if (show != 0)
        print_board(board, *side);
    if (test_end(board, *unplayed) == 1)
        return 0;
    if (test_possible_moves(board, *side, flips) == 0) {
       *unplayed += 1;
       *side *= -1;
       return 1;
    }
    if (*side == BLACK) {
        move = get_move(board, BLACK, black_source, *unplayed);
        legal_move(board, move, BLACK, flips);
        make_move(board,move,BLACK,flips);
    }
    else if (*side == WHITE) {
        move = get_move(board, WHITE, white_source, *unplayed);
        legal_move(board, move, WHITE, flips);
        make_move(board, move, WHITE, flips);
    }
    *side *= -1;
    *unplayed = 0;
    return 1;
}

void empty_board(int *board) {
    /* Make array board an empty board
     * Empty board contains only EMPTY and BORDER squares
     */
    int i;
    for (i=100; i>0; --i) {
        if (i % 10 == 9 || i % 10 == 0 || i < 9 || i>90) {
            board[i] = BORDER;
        }
        else {
            board[i] = EMPTY;
        }
    }
}

void default_board(int *board) {
    /* Empty the board and then set default starting positions
     */
    empty_board(board);
    // set starting locations
    board[44] = board[55] = WHITE;
    board[45] = board[54] = BLACK;
}

void print_board(int *board, int side) {
    /* Print the board to the screen in a human-readable form
     * Includes row and column markers
     * Also indicates possible playing positions and current scores
     */
    int i;
    int flips[19];
    if (side == BLACK)
        printf("Currently black's turn to play");
    else printf("Currently white's turn to play");
    printf("\n\t");
    for(i=1; i<89; ++i) {
        if (i <= 8)
            printf("%d   ",i);
        else if (i % 10 == 0)
            printf("%d\t",i);
        else if (i % 10 == 9)
            // horizontal line
            printf("\n---------------------------------------\n");
        else {
            if (board[i] == WHITE)
                printf("W   ");
            else if (board[i] == BLACK)
                printf("B   ");
            else if (board[i] == EMPTY) {
                if (legal_move(board,i,side,flips) == 1)
                    // indicates that the move is playable
                    printf("-   ");
                else
                    printf("    ");
            }
        }
    }
    printf("\n\nCurrent score:\n");
    printf("Black: %d\nWhite: %d\n",
            find_score(board,BLACK), find_score(board,WHITE));
}

void print_victor(int *board) {
    /* Print a message showing who won and the score of the game
     */
    int white_score = find_score(board, WHITE);
    int black_score = find_score(board, BLACK);
    if (white_score > black_score)
        printf("White has won with a score of %d to %d.\n", white_score,
                black_score);
    else if (black_score > white_score)
        printf("Black has won with a score of %d to %d.\n", black_score,
                white_score);
    else
        printf("The game is tied with a score of %d to %d.\n", white_score,
                black_score);
}
