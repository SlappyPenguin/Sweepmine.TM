#include "header.h"
#include <bits/stdc++.h>
using namespace std;

int num_rows, num_cols, num_bombs;
vector<int> grid;

static void read_input() {
    freopen("in.txt", "r", stdin), freopen("out.txt", "w", stdout);
    cin >> num_rows >> num_cols >> num_bombs;
    int num_squares = num_rows * num_cols;
    grid.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) cin >> grid[i];
}
static void print_move(Move move) { cout << move.is_flag << " " << move.square.row << " " << move.square.col << '\n'; }

int main() {
    read_input();

    Move answer = do_logic();
    if (!answer.is_valid()) {
        make_constraints();
        answer = do_gauss();
        if (!answer.is_valid()) answer = do_probability();
    } 
    print_move(answer);
}