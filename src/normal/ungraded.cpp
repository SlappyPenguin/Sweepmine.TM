/*
Used for debugging purposes. Instead of being linked to the grader, it reads an input grid from data/input.txt and 
writes the next move to data/output.txt.
*/

#include "../../include/normal.h"
#include <bits/stdc++.h>
using namespace std;

constexpr int HIDDEN_VALUE = 100;
constexpr int FLAGGED_VALUE = 101;
const string INPUT_FILE = "../data/input.txt";
const string OUTPUT_FILE = "../data/output.txt";

int num_rows, num_cols, num_bombs;
vec<Cell> grid;

void read_input() {
    ifstream file(INPUT_FILE);
    file >> num_rows >> num_cols >> num_bombs;
    int num_squares = get_num_squares();
    grid.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) {
        int value; file >> value;
        switch (value) {
            case HIDDEN_VALUE: grid[i].state = State::Hidden; break;
            case FLAGGED_VALUE: grid[i].state = State::Flagged; break;
            default: grid[i].state = State::Revealed, grid[i].num_adj_bombs = value; 
        }
    }
}
void print_move(Move move) { 
    ofstream file(OUTPUT_FILE);
    file << move.is_flag << " " << move.square.row << " " << move.square.col << '\n'; 
}

void init() {
    num_rows = num_cols = num_bombs = 0;
    grid.clear();
}

int main() {
    init();
    read_input();

    auto [found, move] = do_logic();
    if (!found) {
        make_constraints();
        tie(found, move) = do_gauss();
    }
    if (!found) {
        move = do_probability();
    }
    print_move(move);
}