#include "tictactoe.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <cassert>
using namespace std;

// Changing the random seed will impact the moves that the tutors make
static const int RANDOM_SEED = 1;

static const int STUDENT = 1, GRADER = 2;
static vector<pair<pair<int, int>, char> > all_moves;
static bool DEBUG_MODE;
static int n;

void end_program(bool error) {
    if (DEBUG_MODE || error) {
        printf("Printing game summary to tictactoe.in\n");
        FILE* out = fopen("tictactoe.in", "w");
        fprintf(out, "%d %lu\n", n, all_moves.size());
        for (auto a : all_moves) {
            fprintf(out, "%d %d %c\n", a.first.first, a.first.second, a.second);
        }
    }
    exit(0);
}

// Returns grids which store the longest chain ending at a cell, coming from the left, right, up, down respectively
static array<vector<vector<int> >, 4> getLeftRightUpDown(const vector<vector<int> > &grid) {
    auto left = vector<vector<int> >(n, vector<int>(n, 0));
    auto right = left, up = left, down = left;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == STUDENT) {
                left[i][j] = 1;
                up[i][j] = 1;
                if (j) left[i][j] += left[i][j-1];
                if (i) up[i][j] += up[i-1][j];
            }
        }
    }

    for (int i = n-1; i >= 0; i--) {
        for (int j = n-1; j >= 0; j--) {
            if (grid[i][j] == STUDENT) {
                right[i][j] = 1;
                down[i][j] = 1;
                if (j != n-1) right[i][j] += right[i][j+1];
                if (i != n-1) down[i][j] += down[i+1][j];
            }
        }
    }

    return { left, right, up, down };
}

static vector<pair<int, int>> getPossibleMoves(const vector<vector<int> > &grid, int subtask) {
    auto lrup = getLeftRightUpDown(grid);
    auto left = lrup[0], right = lrup[1], up = lrup[2], down = lrup[3];

    int mx = 0;
    vector<pair<int, int> > moves;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j]) continue;
            int score = 0;
            if (j) score = max(score, left[i][j-1]);
            if (i) score = max(score, up[i-1][j]);
            if (j != n-1) score = max(score, right[i][j+1]);
            if (i != n-1) score = max(score, down[i+1][j]);

            if (subtask == 1) {
                score = min(score, 1);
            }

            if (score > mx) {
                mx = score;
                moves.clear();
            }

            if (score == mx) moves.push_back({i, j});
        }
    }

    return moves;
}

static vector<vector<int> > grid;
static int num_moves, strategy;
static mt19937 rng;

pair<int, int> make_move(pair<int, int> move) {
    if (num_moves >= n*n) {
        printf("Error: make_move called after game is over\n");
        end_program(true);
    }
    if (move.first < 0 || move.second < 0 || move.first >= n || move.second >= n) {
        printf("Error: Invalid indices in make_move: %d %d\n", move.first, move.second);
        end_program(true);
    }
    if (grid[move.first][move.second]) {
        printf("Error: make_move call with non-empty cell: %d %d\n", move.first, move.second);
        end_program(true);
    }

    all_moves.push_back({move, 'O'});
    grid[move.first][move.second] = STUDENT;
    num_moves++;

    if (num_moves == n*n) return { -1, -1 };
    auto moves = getPossibleMoves(grid, strategy);
    assert(!moves.empty());

    auto our_move = moves[uniform_int_distribution<int>(0, (int)moves.size()-1)(rng)];
    assert(!grid[our_move.first][our_move.second]);
    grid[our_move.first][our_move.second] = GRADER;
    num_moves++;
    all_moves.push_back({our_move, 'X'});
    return our_move;
}

int main() {
    rng = mt19937(RANDOM_SEED);

    int t;
    scanf("%d%d%d", &n, &strategy, &t);
    if (strategy != 1 && strategy != 2) {
        printf("Strategy must be 1 or 2, not %d\n", strategy);
        return 0;
    }

    if (n <= 0 || t < 0) {
        printf("Invalid input\n");
        return 0;
    }
    
    if (t == 0) {
        printf("Running in debug mode\n");
        DEBUG_MODE = 1;
        t = 1;
    }

    vector<int> scores;

    for (int game = 1; game <= t; game++) {
        all_moves.clear();

        grid = vector<vector<int> >(n, vector<int>(n, 0));
        num_moves = 0;

        play_game(n, strategy);
        if (num_moves != n*n) {
            printf("Error: play_game returned before game ended\n");
            end_program(true);
        }

        int score = 0;
        auto lrup = getLeftRightUpDown(grid);

        for (auto &a : lrup) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    score = max(score, a[i][j]);
                }
            }
        }

        scores.push_back(score);
    }

    double ave = accumulate(scores.begin(), scores.end(), 0) / (double)t;

    printf("Average score: %.5lf\n", ave);
    if (strategy == 1) {
        printf("This would earn you approximately %.2lf points for subtask 1\n", ave);
    } else {
        double score = 0;
        if (ave >= 25) score = 75;
        else if (ave > 12) score = 55 + 20*(ave-12)/13.0;
        else if (ave > 8) score = 35 + 20*(ave-8)/4.0;
        else if (ave > 3) score = 35*(ave-3)/5;
        printf("This would earn you approximately %.2lf points for subtask 2\n", score);
    }

    end_program(false);
}