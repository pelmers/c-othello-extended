#include "c-othello.h"
#include "board.c"
#include "ai.c"

// Peter Elmers, September 2012

int get_source(int side) {
    /* Read source selection from standard input (user)
     * Return integer representing choice
     */
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
    /* Get number of games to simulate
     * Return integer
     */
    char input[16];
    printf("Number of games to simulate: ");
    scanf("%s",input);
    return atoi(input);
}

int get_randomize() {
    /* Ask in standard input whether to randomize start of games
     * Return integer, 0 being false, others true
     */
    char input[16];
    printf("Do you want to randomize game starts? (1/0 for yes/no) ");
    scanf("%s",input);
    return atoi(input);
}

int get_human_move(int *board, int side) {
    /* Return human move based on input
     */
    int move;
    char input[32];
    int flips[20];
    printf("Possible moves: ");
    for(int i=11; i<89; ++i) {
        if (legal_move(board,i,side,flips) == 1)
            printf(" %d",i);
    }
    printf("\n");
    for ever {
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

void progress_bar(int width, int percent) {
    /* Print a progress bar using width of screen and perecentage filled
     */
    int i;
    int filled;
    width -= 9;
    filled = width*percent/100;
    printf("\r[ ");
    for(i=0;i < filled; ++i)
        printf("#");
    while((i++) < width)
        printf("-");
    printf(" ] %d%%", percent);
}


int main () {
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
    int flips[20];
    int randomize = 0;
    int side = BLACK;
    int unplayed = 0;
    int simulate = 0;
    int black_source = get_source(BLACK);
    int white_source = get_source(WHITE);
    // initialize seed for rand()
    srand(time(NULL));
    // necessary for progress_bar to work well
    setvbuf(stdout, NULL, _IONBF, 0);
        if (black_source != HUMAN && white_source != HUMAN) {
        simulate = get_simulate_number();
        if (black_source != RANDOM || white_source != RANDOM) {
            randomize = get_randomize();
        }
    }

    if (simulate < 2) {
        // don't need to run full simulation for just one game
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
        int percent = 0, old_percent = 0;
        start = clock();
        for(int i=0;i<simulate;++i) {
            percent = i*100/simulate;
            if (percent > old_percent) {
                progress_bar(SCR_WIDTH,percent);
                old_percent = percent;
            }
            default_board(board);
            side = BLACK;
            unplayed = 0;
            turn_number = 0;
            playing = 1;
            while (playing) {
                if (randomize != 0 && turn_number++ <= 10) {
                    playing = play_turn(board, &side, &unplayed, 0,
                            RANDOM, RANDOM, flips);
                }
                else {
                    playing = play_turn(board, &side, &unplayed, 0,
                            black_source, white_source, flips);
                }
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
}
