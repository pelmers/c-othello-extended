#include "c-othello.h"

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

//const int weights[100] = {
    //0,   0,   0,  0,  0,  0,  0,   0,   0,  0,
    //0, 500,-240, 85, 69, 69, 85,-240, 500,  0,
    //0,-240,-130, 49, 23, 23, 49,-130,-240,  0,
    //0,  85,  49,  1,  9,  9,  1,  49,  85,  0,
    //0,  69,  23,  9, 32, 32,  9,  23,  69,  0,
    //0,  69,  23,  9, 32, 32,  9,  23,  69,  0,
    //0,  85,  49,  1,  9,  9,  1,  49,  85,  0,
    //0,-240,-130, 49, 23, 23, 49,-130,-240,  0,
    //0, 500,-240, 85, 69, 69, 85,-240, 500,  0,
    //0,   0,   0,  0,  0,  0,  0,   0,   0,  0,
//};

int get_random_move(int *board, int side) {
    /* Randomly pick a move!
     * Mostly used for simulating other AI algorithms
     */
    int i;
    int move;
    int counter = 0;
    int possible_moves[64];
    int flips[24];
    for(i=11; i<89; ++i) {
        if (legal_move(board,i,side,flips)) {
            possible_moves[counter] = i;
            counter++;
        }
    }
    // Using modulus with rand is not ideal, but it's good enough
    // Problem is that it can give skewed results
    move = possible_moves[(rand() % counter)];
    return move;
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
            // victory is max score
            return MAX_SCORE;
        else if (score_opp > score_self)
            // defeat is min score
            return MIN_SCORE;
    }
    for (i=11;i<89;++i) {
        if (board[i] == side)
            score += weights[i];
        else if (board[i] == -side)
            // subtract opponent's scores
            score -= weights[i];
    }
    return score;
}

int get_shallow_move(int *board, int side) {
    /* Return the move that gives the best position immediately after
     */
    int i;
    int move = 0;
    int score;
    int flips[24];
    int old_board[100];
    int best_score = MIN_SCORE;
    // backup the existing board
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
        if (legal_move(board,i,side,flips) == 1) {
            make_move(board,i,side,flips);
            score = evaluate_board(board, side, 0);
            if (score > best_score) {
                best_score = score;
                move = i;
            }
            // reset the board before each iteration
            copy_board(old_board, board);
        }
    }
    return move;
}

// Blasphemous! maximize instead of maximise?!
int maximize(int *board, int side, int unplayed, int ply) {
    /* The max portion of minimax
     * Return maximum score available from position
     * Used with currently playing side's turn
     * Search with mutual recursion to depth ply 
     */
    int old_board[100];
    int flips[24];
    int i;
    int score = MIN_SCORE;
    if (ply == 0 || test_end(board, unplayed) == 1)
        // either reached end of search or game is over, so return
        return evaluate_board(board, side, unplayed);
    // backup the current playing board
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        score = MAX(score, minimize(board, -side, 0, ply-1));
        // reset the board after computing the score
        copy_board(old_board, board);
    }
    return score;
}

int minimize(int *board, int side, int unplayed, int ply) {
    /* Search to depth ply with mutual recursion
     * Side should be the opponent of currently playing side
     * Return the lowest score found
     */
    int old_board[100];
    int flips[24];
    int i;
    int score = MAX_SCORE;
    if (ply == 0 || test_end(board, unplayed) == 1)
        return evaluate_board(board, side, unplayed);
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        score = MIN(score, maximize(board, -side, 0, ply-1));
        copy_board(old_board, board);
    }
    return score;
}

// main difference between get_move and maximize is that this one 
// returns a move, not a score
int get_minimax_move(int *board, int side, int unplayed, int ply) {
    /* Similar to maximize(), but return move instead of score
     */
    int i;
    int score;
    int best_move = 0;
    int flips[24];
    int old_board[100];
    int best_score = MIN_SCORE-2;
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
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
    /* Search to depth ply with mutual recursion
     * Return maximum score possible for current side
     * Cuts off search using a and b parameters if possible
     */
    int old_board[100];
    int flips[24];
    int i;
    if (ply == 0 || test_end(board, unplayed) == 1)
        return evaluate_board(board, side, unplayed);
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        a = MAX(a, ab_minimize(board, -side, 0, ply-1, a, b));
        copy_board(old_board, board);
        if (b <= a)
            // that's a cutoff, no point further pursuing this position
            break;
    }
    return a;
}

int ab_minimize(int *board, int side, int unplayed, int ply,int a, int b) {
    /* Search to depth ply with mutual recursion
     * Return minimum score possible as opponent of current player
     * Cut off with a and b when possible
     */
    int old_board[100];
    int flips[24];
    int i;
    if (ply == 0 || test_end(board, unplayed) == 1)
        return evaluate_board(board, side, unplayed);
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
        if (legal_move(board, i, side, flips) == 0)
            continue;
        make_move(board, i, side, flips);
        b = MIN(b, ab_maximize(board, -side, 0, ply-1, a, b));
        copy_board(old_board, board);
        if (b <= a)
            break;
    }
    return b;
}

int get_alphabeta_move(int *board, int side, int unplayed, int ply) {
    /* Return best move using alphabeta search to ply depth
     * Similar to ab_maximize, but finds move instead of score
     */
    int i;
    int score;
    int best_move = 0;
    int flips[24];
    int old_board[100];
    int best_score = MIN_SCORE-1;
    copy_board(board, old_board);
    for (i=11; i<89; ++i) {
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
