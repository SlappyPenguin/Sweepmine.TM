#include "header.h"
#include <bits/stdc++.h>
using namespace std;

int num_rows, num_cols, num_bombs;
bool is_wrap;
vector<int> grid, weight;

static void read_input() {
    freopen("in.txt", "r", stdin), freopen("out.txt", "w", stdout);
    cin >> num_rows >> num_cols >> num_bombs >> is_wrap;
    int num_squares = num_rows * num_cols;
    grid.resize(num_squares + 1), weight.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) cin >> grid[i];
    for (int i = 1; i <= num_squares; i++) cin >> weight[i];
}
static void print_move(Move move) { cout << move.is_flag << " " << move.square.row << " " << move.square.col << '\n'; }

int main() {
    read_input();

    make_constraints();
    Move answer = do_gauss();
    if (!answer.is_valid()) answer = do_probability();  
    assert(answer.is_valid());
    print_move(answer);
}