#include "header.h"
#include <bits/stdc++.h>
using namespace std;

Move do_logic() {
    Move answer;
    for (int i = 1; i <= num_rows * num_cols; i++) {
        Square square = {i};
        if (grid[square.index()] == BLANK || grid[square.index()] == FLAG) continue;
        if (square.num_adj_blanks() == 0) continue;
        if (square.num_adj_blanks() == grid[square.index()] - square.num_adj_flags()) {
            for (Square new_square : square.adj())
                if (grid[new_square.index()] == BLANK) answer = {true, new_square};
        } else if (grid[square.index()] == square.num_adj_flags()) {
            for (Square new_square : square.adj())
                if (grid[new_square.index()] == BLANK) answer = {false, new_square};
        }
    }
    return answer;
}