#include <bits/stdc++.h>
using namespace std;
const int BLANK = 100, FLAG = 101;
const int INVALID = 404;

extern int num_rows, num_cols, num_bombs;
extern bool is_wrap;
extern vector<int> grid, weight;

struct Square {
    int row, col;
    
    Square() { row = INVALID, col = INVALID; }
    Square(int new_row, int new_col) { row = new_row, col = new_col; }
    Square(int index) { row = ceil(index / (double) num_cols), col = (index % num_cols == 0) ? num_cols : index % num_cols; }
    bool is_valid() { return (row != INVALID); }
    int index() { return (row - 1) * num_cols + col; }
    bool in_grid() { return (1 <= row && row <= num_rows && 1 <= col && col <= num_cols); }
    bool is_corner() { return (row == 1 || row == num_rows) && (col == 1 || col == num_cols); };
    bool is_edge() { return (row == 1 || row == num_rows) || (col == 1 || col == num_cols); };

    vector<Square> adj() {
        vector<Square> answer;
        for (int delta_row : {-1, 0, 1}) {
            for (int delta_col : {-1, 0, 1}) {
                if (delta_row == 0 && delta_col == 0) continue;
                int new_row = is_wrap ? (row + delta_row) % num_rows : row + delta_row;
                int new_col = is_wrap ? (col + delta_col) % num_cols : col + delta_col;
                if (is_wrap) {
                    if (new_row == 0) new_row = num_rows;
                    if (new_col == 0) new_col = num_cols;
                }
                Square new_square = {new_row, new_col}; 
                if (new_square.in_grid()) answer.push_back(new_square);
            }
        }
        return answer;
    }
    int num_adj_blanks() {
        int answer = 0;
        for (Square new_square : adj()) answer += (grid[new_square.index()] == BLANK);
        return answer;    
    }
    int num_adj_flags() {
        int answer = 0;
        for (Square new_square : adj()) answer += (grid[new_square.index()] == FLAG);
        return answer;
    }
};
struct Move {
    bool is_flag;
    Square square;

    Move(bool new_is_flag = false, Square new_square = {INVALID, INVALID}) { is_flag = new_is_flag, square = new_square; }
    bool is_valid() { return square.is_valid(); }
};
struct Constraint {
    vector<Square> vars;
    int target_sum, sum, pos_sum_left, neg_sum_left;
    bool is_valid() { return vars.size(); }
};

extern vector<Constraint> constraints;

Move do_logic();
void make_constraints();
Move do_gauss();
Move do_probability();
