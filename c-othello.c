#include <stdio.h> 
#include <time.h>
#include <stdlib.h>
#include <memory.h>

// Peter Elmers, September 2012

//define a few constants
#define EMPTY 0
#define WHITE 1
#define BLACK -1
#define BORDER 2
#define MAX_SCORE 9999
#define MIN_SCORE -9999

#define HUMAN 0
#define RANDOM 1
#define SHALLOW 2
#define MINIMAX 3
#define ALPHABETA 4

// END_DEPTH controls when the AI begins to search all the way to end
// S_DEPTH is all other times
#define S_DEPTH 3
#define END_DEPTH 8

// macros to find maximum and minimum of two integers
#define max(a, b) (((a) + (b) + abs((a) - (b))) * 0.5)
#define min(a, b) (((a) + (b) - abs((a) - (b))) * 0.5)

int get_source(int side);
int get_simulate_number();
int get_randomize();
void reset_flips(int *flips);
void empty_board(int *board);
void default_board(int *board);
void copy_board(int *oldboard, int *newboard);
void print_board(int *board, int side);
void print_victor(int *board);
void to_flip(int *board, int move, int side, int *flips);
void make_move(int *board, int move, int side, int *flips);
int legal_move(int *board, int move, int side, int *flips);
int flip_possible(int *board, int move, int side);
int test_possible_moves(int *board, int side, int *flips);
int test_end(int *board, int unplayed);
int find_score(int *board, int side);
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
int get_move(int *board, int side, int source, int unplayed);
int play_turn(int *board, int *side, int *unplayed, int show, 
        int black_source, int white_soure, int *flips);
void progress_bar(int width, double percent);
int main();

// borrowed from Norvig's Paradigms of AI programming
const int weights[100] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,120,-20, 20,  5,  5, 20,-20,120,  0,
    0,-20,-40, -5, -5, -5, -5,-40,-20,  0,
    0, 20, -5,  3,  3,  3,  3, -5, 20,  0,
    0,  5, -5,  3,  3,  3,  3, -5,  5,  0,
    0,  5, -5,  3,  3,  3,  3, -5,  5,  0,
    0, 20, -5,  3,  3,  3,  3, -5, 20,  0,
    0,-20,-40, -5, -5, -5, -5,-40,-20,  0,
    0,120,-20, 20,  5,  5, 20,-20,120,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

//right, left, up, down, diagonals
const int directions[8] = {1,-1, 10,-10, 9,-9, 11,-11};

int get_source(int side) {
    int choice;
    char input[16];
    printf("\
1) Human\n\
2) Random\n\
3) Shallow searcher (1-ply)\n\
4) Brute minimax (3-ply)\n\
5) Alphabeta pruner (3-ply)\n");
    if (side == BLACK)
        printf("Source for black player: ");
    else printf("Source for white player: ");
    scanf("%s",input);
    choice = atoi(input);
    return choice-1;
}

int get_simulate_number() {
    char input[16];
    printf("Number of games to simulate: ");
    scanf("%s",input);
    return atoi(input);
}

int get_randomize() {
    char input[16];
    printf("Do you want to randomize game starts? (1/0 for yes/no) ");
    scanf("%s",input);
    return atoi(input);
}

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
    //for (i=0;i<24;i++)
    //    flips[i] = 0;
}

void copy_board(int *oldboard, int *newboard) {
    // again, memcpy from memory.h is faster than loop for copying data
    memcpy(newboard, oldboard, 100*sizeof(int));
    //int i;
    //for (i=0;i<100;i++)
    //    newboard[i] = oldboard[i];
}

void to_flip(int *board, int move, int side, int *flips) {
    // d keeps track of direction being followed
    // n keeps track of index of new_flips[]
    int d, n;
    int next;
    // the most flippable in one direction is 6
    int new_flips[7];
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
                        i++;
                    }
                    break;
                }
            }
        }
    }
}

// Like to_flip function, but returns as soon as it finds a possible flip
int flip_possible(int *board, int move, int side) {
    if (board[move] != EMPTY)
        return 0;
    int d, n;
    int next;
    int new_flips[7];
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
                    // instead of writing the flip, just return
                    return 1;
                }
            }
        }
    }
    return 0;
}

void make_move(int *board, int move, int side, int *flips) {
    /* Make a move
     * typically called after legal_move!
     */
    int i;
    // set a tile at this location
    board[move] = side;
    for (i=0; flips[i] != 0; i++)
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
    for (i=11;i<90;i++) {
        if (flip_possible(board, i, side) == 1)
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
    for (i=11;i<90;i++) {
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
    for (i=11;i<90;i++) {
        if (board[i] == side)
            n++;
    }
    return n;
}

int evaluate_board(int *board, int side, int unplayed) {
    /* Evaluate the current board position for side
     * MAX_SCORE if victory, MIN_SCORE if loss
     * Return the score as sum of own minus sum of opponent's
     */
    int score = 0;
    int i;
    if (test_end(board, unplayed) == 1) {
        int score_self = find_score(board,side);
        int score_opp = find_score(board,-side);
        if (score_self > score_opp)
            return MAX_SCORE;
        else if (score_opp > score_self)
            return MIN_SCORE;
    }
    for (i=11;i<90;i++) {
        if (board[i] == side)
            score += weights[i];
        else if (board[i] == -side)
            score -= weights[i];
    }
    return score;
}


int get_human_move(int *board, int side) {
    /* Return human move based on input
     */
    int i;
    int move;
    char input[32];
    int flips[24];
    printf("Possible moves: ");
    for(i=11; i<90; i++) {
        if (flip_possible(board,i,side) == 1)
            printf(" %d",i);
    }
    printf("\n");
    for (;;) {
        printf("Enter your move (sum of row and column): ");
        scanf("%s",input);
        move = atoi(input);

        //if (fgets(buf, sizeof(buf), stdin) != NULL)
        //    move = atoi(buf);

        printf("\n");
        if (flip_possible(board,move,side) == 1)
            return move;
        else
            printf("Invalid input, please try again\n");
    }
}

int get_random_move(int *board, int side) {
    /* Randomly pick a move!
     * Mostly used for testing
     */
    int i;
    int move;
    int counter = 0;
    int possible_moves[64];
    //int flips[24];
    for(i=11; i<90; i++) {
        if (flip_possible(board,i,side)) {
        //if (legal_move(board,i,side,flips)) {
            possible_moves[counter] = i;
            counter++;
        }
    }
    move = possible_moves[(rand() % counter)];
    return move;
}

int get_shallow_move(int *board, int side) {
    /* Return the move that gives the best position immediately after
     */
    int i;
    int move;
    int score;
    int flips[24];
    int old_board[100];
    int best_score = MIN_SCORE;
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board,i,side,flips) == 1) {
            make_move(board,i,side,flips);
            score = evaluate_board(board, side, 0);
            if (score > best_score) {
                best_score = score;
                move = i;
            }
            copy_board(old_board, board);
        }
    }
    return move;
}

// Blasphemous! maximize instead of maximise?!
int maximize(int *board, int side, int unplayed, int ply) {
    int old_board[100];
    int flips[24];
    int i;
    int score = MIN_SCORE;
    if (ply == 0 || test_end(board, unplayed) == 1)
        // either reached end of search or game is over, so return
        return evaluate_board(board, side, unplayed);
    // backup the current playing board
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        score = max(score, minimize(board, -side, 0, ply-1));
        // reset the board after computing the score
        copy_board(old_board, board);
    }
    return score;
}

int minimize(int *board, int side, int unplayed, int ply) {
    int old_board[100];
    int flips[24];
    int i;
    int score = MAX_SCORE;
    if (ply == 0 || test_end(board, unplayed) == 1)
        return evaluate_board(board, side, unplayed);
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        score = min(score, maximize(board, -side, 0, ply-1));
        copy_board(old_board, board);
    }
    return score;
}

// main difference between get_move and maximize is that this one 
// returns a move, not a score
int get_minimax_move(int *board, int side, int unplayed, int ply) {
    int i;
    int score;
    int best_move;
    int flips[24];
    int old_board[100];
    int best_score = MIN_SCORE-2;
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        score = minimize(board, -side, unplayed, ply);
        copy_board(old_board, board);
        if (score > best_score) {
            best_score = score;
            best_move = i;
        }
    }
    return best_move;
}

int ab_maximize(int *board, int side, int unplayed, int ply, int a,int b) {
    int old_board[100];
    int flips[24];
    int i;
    if (ply == 0 || test_end(board, unplayed) == 1)
        return evaluate_board(board, side, unplayed);
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        a = max(a, ab_minimize(board, -side, 0, ply-1, a, b));
        copy_board(old_board, board);
        if (b <= a)
            // that's a cutoff, no point further pursuing this position
            break;
    }
    return a;
}

int ab_minimize(int *board, int side, int unplayed, int ply,int a, int b) {
    int old_board[100];
    int flips[24];
    int i;
    if (ply == 0 || test_end(board, unplayed) == 1)
        return evaluate_board(board, side, unplayed);
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        b = min(b, ab_maximize(board, -side, 0, ply-1, a, b));
        copy_board(old_board, board);
        if (b <= a)
            break;
    }
    return b;
}

int get_alphabeta_move(int *board, int side, int unplayed, int ply) {
    int i;
    int score;
    int best_move;
    int flips[24];
    int old_board[100];
    int best_score = MIN_SCORE-1;
    copy_board(board, old_board);
    for (i=11; i<90; i++) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        score = ab_minimize(board, -side, unplayed, ply,
                best_score,MAX_SCORE+1);
        copy_board(old_board, board);
        if (score > best_score) {
            best_score = score;
            best_move = i;
        }
    }
    return best_move;
}


void empty_board(int *board) {
    int i;
    for (i=0; i<100; i++) {
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
    for(i=1; i<89; i++) {
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
                if (flip_possible(board,i,side) == 1)
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
        for (i=11;i<90;i++) {
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
        for (i=11;i<90;i++) {
            if (board[i] == EMPTY)
                count++;
        }
        if (count >= END_DEPTH)
            return get_alphabeta_move(board, side, unplayed, S_DEPTH);
        else
            return get_alphabeta_move(board, side, unplayed, count);
    }return 0;
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

void progress_bar(int width, double percent) {
    double filled;
    int i = 0;
    width -= 9;
    filled = (double)width*percent/100.0;
    printf("\r[ ");
    for(i=0;i < (int)filled; i++)
        printf("#");
    for(i=i;i < (width-1); i++)
        printf("-");
    printf(" ] %.0f%%", percent);
}


int main () {
    int i;
    clock_t start;
    clock_t end;
    double elapsed;
    int playing;
    int turn_number;
    int score_w;
    int score_b;
    int wins_w = 0;
    int wins_b = 0;
    int draws = 0;
    int board[100];
    int flips[24];
    int black_source = get_source(BLACK);
    int white_source = get_source(WHITE);
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    int simulate = 0;
    int randomize = 0;
    int side = BLACK;
    int unplayed = 0;
    if (black_source != HUMAN && white_source != HUMAN) {
        simulate = get_simulate_number();
        if (black_source != RANDOM || white_source != RANDOM) {
            randomize = get_randomize();
        }
    }

    if (simulate < 2) {
        default_board(board);
        playing = 1;
        turn_number = 0;

        while (playing) {
            playing = play_turn(board, &side, &unplayed, 1, black_source, 
                    white_source, flips);
            turn_number++;
        }
        print_victor(board);
    }

    else {
        start = clock();
        for(i=0;i<simulate;i++) {
            progress_bar(80,(double)i/(double)simulate*100.0);
            default_board(board);
            side = BLACK;
            unplayed = 0;
            turn_number = 0;
            playing = 1;
            while (playing) {
                if (randomize != 0 && turn_number <= 10) {
                    playing = play_turn(board, &side, &unplayed, 0,
                            RANDOM, RANDOM, flips);
                }
                else {
                    playing = play_turn(board, &side, &unplayed, 0,
                            black_source, white_source, flips);
                }
                turn_number++;
            }
            score_w = find_score(board,WHITE);
            score_b = find_score(board,BLACK);
            if (score_w > score_b)
                wins_w++;
            else if (score_w == score_b)
                draws ++;
            else
                wins_b++;
        }
        end = clock();
        elapsed = (end - start)/(double)CLOCKS_PER_SEC;
        printf("\nBlack %.2f\%\nWhite %.2f\%\nDraw %.2f\%\n",
                ((double)wins_b/(double)simulate*100),
                ((double)wins_w/(double)simulate*100),
                ((double)draws/(double)simulate*100));
        printf("Simulated lasted %.3f seconds, %.4f seconds per game.\n",
                elapsed, elapsed/(double)simulate);
    }
    return 0;
}
