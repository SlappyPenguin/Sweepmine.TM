/*
1) Do trivial checks on whether a square is 100% safe or 100% a mine
2) Do more advanced logic using Gaussian elimination to solve the CSP, determining whether there is a 100% safe or
100% mine square
3) Do backtracking to assign a probability that every square is a mine, then pick the least dangerous square
*/

#include "../../include/normal.h"
#include "../../include/grader.h"
#include <bits/stdc++.h>
using namespace std;

constexpr int HIDDEN_VALUE = 100;
constexpr int FLAGGED_VALUE = 101;

int num_rows, num_cols, num_bombs;
vec<Cell> grid;

void process_input(
    int new_num_rows, int new_num_cols, int new_num_bombs, 
    vec<vec<int>> new_grid
) {
    num_rows = new_num_rows, num_cols = new_num_cols, num_bombs = new_num_bombs;
    int num_squares = get_num_squares();
    grid.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) {
        Square square = {i};
        int value = new_grid[square.row][square.col];
        switch (value) {
            case HIDDEN_VALUE: grid[i].state = State::Hidden; break;
            case FLAGGED_VALUE: grid[i].state = State::Flagged; break;
            default: grid[i].state = State::Revealed, grid[i].num_adj_bombs = value; 
        }
    }
}

void init() {
    num_rows = num_cols = num_bombs = 0;
    grid.clear();
}

pair<bool, pair<int, int>> next_move(
    int new_num_rows, int new_num_cols, int new_num_bombs, 
    vec<vec<int>> new_grid, vec<vec<int>> new_weight, 
    bool new_is_torus
) {
    init();
    process_input(new_num_rows, new_num_cols, new_num_bombs, new_grid);

    auto [found, move] = do_logic();
    if (!found) {
        make_constraints();
        tie(found, move) = do_gauss();
    }
    if (!found) {
        move = do_probability();
    }
    return {move.is_flag, {move.square.row, move.square.col}};
}