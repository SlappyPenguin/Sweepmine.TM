#include "header.h"
#include <bits/stdc++.h>
using namespace std;

vector<Constraint> constraints;
void make_constraints() {
    int num_squares = num_rows * num_cols;
    constraints.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) {
        Square square = {i};
        if (grid[square.index()] == BLANK || grid[square.index()] == FLAG) continue;
        if (square.num_adj_blanks() == 0) continue;
        for (Square new_square : square.adj()) {
            if (grid[new_square.index()] != BLANK) continue;
            constraints[i].vars.push_back(new_square);
            if (weight[new_square.index()] > 0) constraints[i].pos_sum_left += weight[new_square.index()];
            else constraints[i].neg_sum_left += weight[new_square.index()];
        }
        constraints[i].target_sum = grid[i];
        for (Square new_square : square.adj())
            if (grid[new_square.index()] == FLAG) constraints[i].target_sum -= weight[new_square.index()];
    }
}