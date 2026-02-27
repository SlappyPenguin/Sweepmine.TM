/*
Abstracts the board state into a constraint satisfaction problem (CSP). Each non-zero revealed square defines its
own constraint, i.e. that the squares around it (variables based on their weight) must sum to some target.
*/

#include "../../include/weighted_torus.h"
#include <bits/stdc++.h>
using namespace std;

vec<Constraint> constraints;
static void init() {
    constraints.clear();
}
void make_constraints() {
    init();

    int num_squares = get_num_squares();
    constraints.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) {
        Square square = {i};
        const Cell& cell = grid[i];
        Constraint& cons = constraints[i];
        if (cell.state != State::Revealed) continue;
        if (square.get_num_adj_hidden() == 0) continue;

        cons.target_sum = cell.num_adj_bombs;
        for (const Square& new_square : square.get_adj()) {
            int index = new_square.get_index();
            if (grid[index].state != State::Flagged) continue;
            cons.target_sum -= weight[index];
        }

        for (const Square& new_square : square.get_adj()) {
            int index = new_square.get_index();
            const Cell& new_cell = grid[index];
            if (new_cell.state != State::Hidden) continue;
            
            cons.vars.push_back(new_square);
            if (weight[index] > 0) cons.pos_sum_left += weight[index];
            else cons.neg_sum_left += weight[index];
        }
    }
}