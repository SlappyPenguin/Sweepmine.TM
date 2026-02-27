/*
Trivial checks on whether any square is 100% safe or 100% a mine.
*/

#include "../../include/normal.h"
#include <bits/stdc++.h>
using namespace std;

pair<bool, Move> do_logic() {
    pair<bool, Move> ans = {false, {}};
    for (int i = 1; i <= get_num_squares(); i++) {
        Square square = {i};
        const Cell& cell = grid[i];
        if (cell.state == State::Hidden || cell.state == State::Flagged) continue;
        if (square.get_num_adj_hidden() == 0) continue;

        bool is_all_bomb = (square.get_num_adj_hidden() + square.get_num_adj_flags() == cell.num_adj_bombs);
        bool is_all_safe = (square.get_num_adj_flags() == cell.num_adj_bombs);
        if (!is_all_bomb && !is_all_safe) continue;

        for (const Square& new_square : square.get_adj()) {
            const Cell& new_cell = grid[new_square.get_index()];
            if (new_cell.state != State::Hidden) continue; 
            ans = {true, {is_all_bomb, new_square}};
        }
    }
    return ans;
}