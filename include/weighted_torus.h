#include <bits/stdc++.h>
using namespace std;
using ldoub = long double;
template<typename T, size_t N> 
using arr = array<T, N>;
template<typename T> 
using vec = vector<T>; 
template<typename T> 
using hset = unordered_set<T>; 

constexpr ldoub EPSILON = 0.995;
constexpr ldoub INF = 1e18;
enum class State : int {
    Hidden, Revealed, Flagged
};

extern int num_rows, num_cols, num_bombs;
extern bool is_torus;
extern vec<int> weight;

inline pair<int, int> get_coords(int row, int col) {
    if (!is_torus) return {row, col};
    row = (row % num_rows == 0) ? num_rows : row % num_rows;
    col = (col % num_cols == 0) ? num_cols : col % num_cols;
    return {row, col};
}
inline int get_num_squares() {
    return num_rows * num_cols;
}
inline bool is_equal(ldoub x, ldoub y) {
    if (x > y) swap(x, y);
    if (y == 0) return true;
    return (x / y >= EPSILON);
}
inline void append_hset(hset<int>& base, const hset<int>& to_append) {
    base.insert(to_append.begin(), to_append.end());
}
inline void append_vec(vec<int>& base, const vec<int>& to_append) {
    base.insert(base.end(), to_append.begin(), to_append.end());
}
inline void pop_vec(vec<int>& base, const vec<int>& to_pop) {
    base.erase(base.end() - to_pop.size(), base.end());
}


struct Cell {
    State state;
    int num_adj_bombs;
    Cell() 
        : state(State::Hidden), num_adj_bombs(0)
    {}
};
extern vec<Cell> grid;

struct Square {
    int row, col;
    Square(int new_row, int new_col) 
        : row(new_row), col(new_col) 
    {}
    Square(int index) 
        : row((index + num_cols - 1) / num_cols),
        col((index % num_cols == 0) ? num_cols : index % num_cols)
    {}

    int get_index() const {
        return (row - 1) * num_cols + col;
    }
    bool is_in_grid() const {
        return (1 <= row && row <= num_rows && 1 <= col && col <= num_cols);
    }
    bool is_corner() const {
        return (row == 1 || row == num_rows) && (col == 1 || col == num_cols);
    }
    bool is_edge() const {
        return (row == 1 || row == num_rows) || (col == 1 || col == num_cols);
    }

    vec<Square> get_adj() const {
        vec<Square> answer;
        for (int delta_row : {-1, 0, 1}) {
            for (int delta_col : {-1, 0, 1}) {
                if (delta_row == 0 && delta_col == 0) continue;
                pair<int, int> coords = get_coords(row + delta_row, col + delta_col);
                Square new_square = {coords.first, coords.second};
                if (!new_square.is_in_grid()) continue;
                answer.push_back(new_square);
            }
        }
        return answer;
    }
    int get_num_adj_hidden() const {
        int answer = 0;
        for (const Square& new_square : get_adj()) 
            answer += (grid[new_square.get_index()].state == State::Hidden);
        return answer;    
    }
    int get_num_adj_flags() const {
        int answer = 0;
        for (const Square& new_square : get_adj()) 
            answer += (grid[new_square.get_index()].state == State::Flagged);
        return answer;
    }
};

struct Move {
    bool is_flag;
    Square square;
    Move() 
        : is_flag(false), square({0, 0})
    {}
    Move(bool new_is_flag, Square new_square) 
        : is_flag(new_is_flag), square(new_square)
    {}
};

struct Constraint {
    vec<Square> vars;
    int sum, target_sum;
    int pos_sum_left, neg_sum_left;
    bool is_empty() const {
        return vars.empty();
    }
};
extern vec<Constraint> constraints;

void make_constraints();
pair<bool, Move> do_gauss();
Move do_probability();
