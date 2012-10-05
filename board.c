#include "c-othello.h"

//right, left, up, down, diagonals
const int directions[8] = {1,-1, 10,-10, 9,-9, 11,-11};

void reset_flips(int *flips) {
    // manually resetting turns out faster than using a loop
    flips[0] = 0;
    flips[1] = 0;
    flips[2] = 0;
    flips[3] = 0;
    flips[4] = 0;
    flips[5] = 0;
    flips[6] = 0;
    flips[7] = 0;
    flips[8] = 0;
    flips[9] = 0;
    flips[10] = 0;
    flips[11] = 0;
    flips[12] = 0;
    flips[13] = 0;
    flips[14] = 0;
    flips[15] = 0;
    flips[16] = 0;
    flips[17] = 0;
    flips[18] = 0;
    flips[19] = 0;
    flips[20] = 0;
    flips[21] = 0;
    flips[22] = 0;
    flips[23] = 0;
    //int i;
    //for (i=0;i<24;++i)
    //    flips[i] = 0;
}

void copy_board(int *oldboard, int *newboard) {
    // again, memcpy from memory.h is faster than loop for copying data
    memcpy(newboard, oldboard, 100*sizeof(int));
    //int i;
    //for (i=0;i<100;++i)
    //    newboard[i] = oldboard[i];
}

void to_flip(int *board, int move, int side, int *flips) {
    // d keeps track of direction being followed
    // n keeps track of index of new_flips[]
    int d, n;
    int next;
    // the most flippable in one direction is 6
    int new_flips[8];
    // i keeps track of index in flips[]
    int i = 0;
    for (d=0; d<8; d++) {
        next = move + directions[d];
        n = 0;
        if (board[next] == -side) {
            new_flips[n] = next;
            for (;;) {
                next += directions[d];
                if (board[next] == EMPTY || board[next] == BORDER)
                    break;
                if (board[next] == -side) {
                    n++;
                    new_flips[n] = next;
                    continue;
                }
                if (board[next] == side) {
                    for (; n>=0; n--) {
                        flips[i] = new_flips[n];
                        ++i;
                    }
                    break;
                }
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

int legal_move(int *board, int move, int side, int *flips) {
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

int test_possible_moves(int *board, int side, int *flips) {
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

int test_end(int *board, int unplayed) {
    /* Return 1 if the game has ended
     * conditions: two consecutive unplayed turns or no empty spaces left
     */
    if (unplayed == 2)
        return 1;
    int i;
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
    int i;
    int n = 0;
    for (i=11;i<89;++i) {
        if (board[i] == side)
            n++;
    }
    return n;
}

int get_move(int *board, int side, int source, int unplayed) {
    if (source == HUMAN)
        return get_human_move(board, side);
    if (source == RANDOM)
        return get_random_move(board, side);
    if (source == SHALLOW)
        return get_shallow_move(board, side);
    if (source == MINIMAX) {
        int i;
        int count = 0;
        for (i=11;i<89;++i) {
            if (board[i] == EMPTY)
                count++;
        }
        if (count >= END_DEPTH)
            return get_minimax_move(board, side, unplayed, S_DEPTH);
        else
            return get_minimax_move(board, side, unplayed, count);
    }
    if (source == ALPHABETA) {
        int i;
        int count = 0;
        for (i=11;i<89;++i) {
            if (board[i] == EMPTY)
                count++;
        }
        if (count >= END_DEPTH)
            return get_alphabeta_move(board, side, unplayed, S_DEPTH);
        else
            return get_alphabeta_move(board, side, unplayed, count);
    }
    return 0;
}


int play_turn(int *board, int *side, int *unplayed, int show, 
        int black_source, int white_source, int *flips) {
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
        move = get_move(board, *side, black_source, *unplayed);
        legal_move(board, move, *side, flips);
        make_move(board,move,*side,flips);
    }
    else if (*side == WHITE) {
        move = get_move(board, *side, white_source, *unplayed);
        legal_move(board, move, *side, flips);
        make_move(board, move, *side, flips);
    }
    *side *= -1;
    *unplayed = 0;
    return 1;
}

void empty_board(int *board) {
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
    empty_board(board);
    // set starting locations
    board[44] = board[55] = WHITE;
    board[45] = board[54] = BLACK;
}

void print_board(int *board, int side) {
    int i;
    int flips[24];
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
            printf("\n---------------------------------------\n");
        else {
            if (board[i] == WHITE)
                printf("W   ");
            else if (board[i] == BLACK)
                printf("B   ");
            else if (board[i] == EMPTY) {
                if (legal_move(board,i,side,flips) == 1)
                    printf("-   ");
                else
                    printf("    ");
            }
        }
    }
    printf("\nCurrent score:\n");
    printf("Black: %d\nWhite: %d\n",
            find_score(board,BLACK), find_score(board,WHITE));
}

void print_victor(int *board) {
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