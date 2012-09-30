#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define EMPTY 0
#define WHITE 1
#define BLACK -1
#define BORDER 2
#define MAX_SCORE 9999
#define MIN_SCORE -9999

#define HUMAN 0
#define RANDOM 1

int get_source(int side);
int get_simulate_number();
void reset_flips(int *flips);
void empty_board(int *board);
void default_board(int *board);
void print_board(int *board, int side);
void print_victor(int *board);
void to_flip(int *board, int move, int side, int *flips);
void make_move(int *board, int move, int side, int *flips);
int legal_move(int *board, int move, int side, int *flips);
int test_possible_moves(int *board, int side, int *flips);
int test_end(int *board, int unplayed);
int find_score(int *board, int side);
int get_human_move(int *board, int side);
int get_random_move(int *board, int side);
int get_move(int *board, int side, int source);
int play_turn(int *board, int *side, int *unplayed, int show, 
        int black_source, int white_soure, int *flips);
int main();

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
    printf("\n");
    return choice-1;
}

int get_simulate_number() {
    char input[16];
    printf("Number of games to simulate: ");
    scanf("%s",input);
    return atoi(input);
}

void reset_flips(int *flips) {
    int i;
    for (i=0;i<24;i++)
        flips[i] = 0;
}

void to_flip(int *board, int move, int side, int *flips) {
    int d, n;
    int next;
    int new_flips[8];
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

void make_move(int *board, int move, int side, int *flips) {
    int i;
    board[move] = side;
    for (i=0; flips[i] != 0; i++)
        board[flips[i]] = side;
}

int legal_move(int *board, int move, int side, int *flips) {
    if (board[move] != EMPTY)
        return 0;
    reset_flips(flips);
    to_flip(board, move, side, flips);
    if (flips[0] == 0)
        return 0;
    return 1;
}

int test_possible_moves(int *board, int side, int *flips) {
    int i;
    for (i=11;i<90;i++) {
        if (legal_move(board, i, side, flips) == 1)
            return 1;
    }
    return 0;
}

int test_end(int *board, int unplayed) {
    if (unplayed == 2)
        return 1;
    int i;
    for (i=11;i<90;i++) {
        if (board[i] == EMPTY)
            return 0;
    }
    return 1;
}

int find_score(int *board, int side) {
    int i;
    int n = 0;
    for (i=11;i<90;i++) {
        if (board[i] == side)
            n++;
    }
    return n;
}

int get_human_move(int *board, int side) {
    int i;
    int move;
    char input[32];
    int flips[24];
    printf("Possible moves: ");
    for(i=10; i<90; i++) {
        if (legal_move(board,i,side,flips) == 1)
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
        if (legal_move(board,move,side,flips) == 1)
            return move;
        else
            printf("Invalid input, please try again\n");
    }
}

int get_random_move(int *board, int side) {
    int i;
    int move;
    int counter = 0;
    int possible_moves[64];
    int flips[24];
    for(i=10; i<90; i++) {
        if (legal_move(board,i,side,flips) == 1) {
            possible_moves[counter] = i;
            counter++;
        }
    }
    move = possible_moves[(rand() % counter)];
    return move;
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

int get_move(int *board, int side, int source) {
    if (source == HUMAN)
        return get_human_move(board, side);
    if (source == RANDOM)
        return get_random_move(board, side);
    return 0;
}


int play_turn(int *board, int *side, int *unplayed, int show, 
        int black_source, int white_source, int *flips) {
    int move;
    if (show != 0)
        print_board(board, *side);
    if (test_possible_moves(board, *side, flips) == 0) {
       *unplayed += 1;
       *side *= -1;
    }
    if (test_end(board, *unplayed) == 1)
        return 0;
    if (*side == BLACK) {
        move = get_move(board, *side, black_source);
        legal_move(board, move, *side, flips);
        make_move(board,move,*side,flips);
    }
    else if (*side == WHITE) {
        move = get_move(board, *side, white_source);
        legal_move(board, move, *side, flips);
        make_move(board, move, *side, flips);
    }
    *side *= -1;
    *unplayed = 0;
    return 1;
}


int main () {
    int i;
    clock_t start;
    clock_t end;
    double elapsed;
    int playing;
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
    int simulate = 0;
    int side = BLACK;
    int unplayed = 0;
    if (black_source != HUMAN && white_source != HUMAN)
        simulate = get_simulate_number();

    if (simulate < 2) {
        default_board(board);
        playing = 1;

        while (playing) {
            playing = play_turn(board, &side, &unplayed, 1, black_source, 
                    white_source, flips);
        }
        print_victor(board);
    }

    else {
        start = clock();
        for(i=0;i<simulate;i++) {
            printf("%d\n",i);
            default_board(board);
            side = BLACK;
            unplayed = 0;
            playing = 1;
            while (playing) {
                playing = play_turn(board, &side, &unplayed, 0,
                        black_source, white_source, flips);
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
        printf("Black %.2f\%\nWhite %.2f\%\nDraw %.2f\%\n",
                ((double)wins_b/(double)simulate*100),
                ((double)wins_w/(double)simulate*100),
                ((double)draws/(double)simulate*100));
        printf("Simulated lasted %.3f seconds, %.3f seconds per game.\n",
                elapsed, elapsed/(double)simulate);
    }
    return 0;
}
