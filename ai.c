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


int get_random_move(int *board, int side) {
    /* Randomly pick a move!
     * Mostly used for testing
     */
    int i;
    int move;
    int counter = 0;
    int possible_moves[64];
    int flips[24];
    //int flips[24];
    for(i=11; i<89; ++i) {
        if (legal_move(board,i,side,flips)) {
            possible_moves[counter] = i;
            counter++;
        }
    }
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
            return MAX_SCORE;
        else if (score_opp > score_self)
            return MIN_SCORE;
    }
    for (i=11;i<89;++i) {
        if (board[i] == side)
            score += weights[i];
        else if (board[i] == -side)
            score -= weights[i];
    }
    return score;
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
    for (i=11; i<89; ++i) {
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
    for (i=11; i<89; ++i) {
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
    for (i=11; i<89; ++i) {
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
    for (i=11; i<89; ++i) {
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
