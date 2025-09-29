#pragma GCC optimize("O3", "unroll-loops")
#pragma GCC target("avx2", "bmi", "bmi2", "lzcnt", "popcnt")

#include <bits/stdc++.h>
using namespace std;
const int BLANK = 100, FLAG = 101;
const int INVALID = 404;

int num_rows, num_cols, num_bombs;
vector<int> grid;

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
                Square new_square = {row + delta_row, col + delta_col}; 
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
    int num_fixed, sum, target_sum;
    bool is_valid() { return vars.size(); }
};

vector<Constraint> constraints;

Move do_logic();
void make_constraints();
Move do_gauss();
Move do_probability();

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

void make_constraints() {
    int num_squares = num_rows * num_cols;
    constraints.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) {
        Square square = {i};
        if (grid[square.index()] == BLANK || grid[square.index()] == FLAG) continue;
        if (square.num_adj_blanks() == 0) continue;
        for (Square new_square : square.adj()) 
            if (grid[new_square.index()] == BLANK) constraints[i].vars.push_back(new_square);
        constraints[i].target_sum = grid[i] - square.num_adj_flags();
    }
}

static int num_vars, num_cons;
static vector<int> gauss_vars, gauss_vars_rev;
static vector<int> gauss_cons;

static void gauss_setup() {
    int num_squares = num_rows * num_cols;
    set<int> var_s;
    gauss_cons.push_back(0); // needs to be cleared in init!!!
    for(int i = 1; i <= num_squares; i++) {
        if(!constraints[i].is_valid()) continue;
        num_cons++;
        gauss_cons.push_back(i);
        for(Square s : constraints[i].vars) var_s.insert(s.index());
    }
    num_vars = var_s.size();
    gauss_vars.resize(num_vars + 1, 0);
    gauss_vars_rev.resize(num_squares + 1, 0);
    for(int i = 1; i <= num_vars; i++) {
        gauss_vars[i] = *var_s.begin();
        gauss_vars_rev[gauss_vars[i]] = i;
        var_s.erase(var_s.begin());
    }
}
static vector<vector<long double>> mat; // wow because this couldnt possibly go wrong
static void construct_augment() {
    mat.resize(num_cons + 2, vector<long double>(num_vars + 2, 0));
    for(int i = 1; i <= num_cons; i++) {
        for(Square s : constraints[gauss_cons[i]].vars) {
            mat[i][gauss_vars_rev[s.index()]] = 1;
        }
    }
    for(int i = 1; i <= num_cons; i++) {
        Square s = {gauss_cons[i]};
        mat[i][num_vars + 1] = grid[gauss_cons[i]] - s.num_adj_flags();
    }
}
static void gauss() {
    int r = num_cons, c = num_vars + 1;
    int j = 1;
    for(int curr = 1; curr <= r; curr++) {
        if(j > c) return;
        int i = curr;
        while(!mat[i][j]) {
            i++;
            if(i > r) {
                i = curr, j++;
                if(j > c) return;
            }
        }
        swap(mat[i], mat[curr]);
        int val = mat[curr][j];
        for(int id = 1; id <= c; id++) mat[curr][id] /= val;
        for(int row = 1; row <= r; row++) {
            if(row == curr) continue;
            val = mat[row][j];
            for(int id = 1; id <= c; id++) mat[row][id] -= val * mat[curr][id];
        }
        j++;
    }
}
static vector<int> var_vals;
static void backsub() {
    var_vals.resize(num_vars + 1, -1);
    for(int i = num_cons; i >= 1; i--) {
        int lb_value = 0, ub_value = 0;
        for(int j = 1; j <= num_vars; j++) {
            if(var_vals[j] != -1) {
                lb_value += var_vals[j] * mat[i][j];
                ub_value += var_vals[j] * mat[i][j];
            } else {
                if(mat[i][j] < 0) lb_value += mat[i][j];
                else ub_value += mat[i][j];
            }
        }
        if(mat[i][num_vars + 1] == lb_value) {
            for(int j = 1; j <= num_vars; j++) {
                if(mat[i][j] == 0 || var_vals[j] != -1) continue;
                var_vals[j] = (mat[i][j] < 0 ? 1 : 0);
            }
        } else if(mat[i][num_vars + 1] == ub_value) {
            for(int j = 1; j <= num_vars; j++) {
                if(mat[i][j] == 0 || var_vals[j] != -1) continue;
                var_vals[j] = (mat[i][j] < 0 ? 0 : 1);
            }
        }
    }
}
static void print_augment() {
    for(int i = 1; i <= num_cons; i++) {
        for(int j = 1; j <= num_vars + 1; j++) {
            cerr << mat[i][j] << " ";
        }
        cerr << "\n";
    }
}
Move do_gauss() {
    Move answer;

    gauss_setup();
    construct_augment();
    gauss();
    // print_augment();
    backsub();

    for(int i = 1; i <= num_vars; i++) {
        if(var_vals[i] == -1) continue;
        Square s = {gauss_vars[i]};
        answer = {(bool) var_vals[i], {s.row, s.col}};
    }

    return answer;
}


static vector<vector<int>> adj;
static int num_comps;
static vector<int> comp;
static void dfs(int u) {
    comp[u] = num_comps;
    for (int v : adj[u])
        if (comp[v] == 0) dfs(v);
}
static void make_graph() {
    int num_squares = num_rows * num_cols;
    adj.resize(num_squares + 1), comp.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) 
        for (Square new_square : constraints[i].vars) 
            adj[i].push_back(new_square.index()), adj[new_square.index()].push_back(i);
    for (int i = 1; i <= num_squares; i++) {
        if (!constraints[i].is_valid()) continue;
        if (comp[i] != 0) continue;
        num_comps++, dfs(i);
    }
}

static vector<vector<int>> constraint_sets; // Stores index 
static void make_sets() {
    constraint_sets.resize(num_comps + 1);
    int num_squares = num_rows * num_cols;
    for (int i = 1; i <= num_squares; i++) {
        if (!constraints[i].is_valid()) continue;
        constraint_sets[comp[i]].push_back(i);
    }
}
static void print_sets() {
    for (int i = 1; i <= num_comps; i++) {
        cout << "SET " << i << ": ";;
        for (int j : constraint_sets[i]) cout << j << " ";
        cout << endl;
    }
}

static vector<vector<vector<int>>> configs;
static vector<pair<int, bool>> vars; // bool: is this var set?
static unordered_set<int> total_vars;
static void backtrack(int comp, int index) {
    if (index == (int) vars.size()) {
        vector<int> answer;
        for (pair<int, bool> var : vars)
            if (var.second) answer.push_back(var.first);
        configs[comp].push_back(answer);
        return;
    }
    int var = vars[index].first;
    for (bool value : {false, true}) {
        int halt_constraint = 0;
        for (int i : adj[var]) {
            Square square = {i};
            constraints[i].num_fixed++, constraints[i].sum += value;
            int maximal_sum = constraints[i].sum + (int) constraints[i].vars.size() - constraints[i].num_fixed;
            if (constraints[i].sum > constraints[i].target_sum) halt_constraint = i;
            if (maximal_sum < constraints[i].target_sum) halt_constraint = i;
            if (halt_constraint != 0) break;
        }
        if (halt_constraint != 0) {
            for (int i : adj[var]) {
                constraints[i].num_fixed--, constraints[i].sum -= value;
                if (i == halt_constraint) break;
            }
            continue;
        }
        vars[index].second = value;
        backtrack(comp, index + 1);
        for (int i : adj[var]) 
            constraints[i].num_fixed--, constraints[i].sum -= value;
    }
}
static void make_configs() {
    configs.resize(num_comps + 1);
    for (int i = 1; i <= num_comps; i++) {
        unordered_set<int> var_set;
        for (int j : constraint_sets[i])
            for (Square square : constraints[j].vars)
                var_set.insert(square.index());
        
        total_vars.insert(var_set.begin(), var_set.end());
        vars.resize(var_set.size() + 1);
        for (int j = 1; j <= (int) vars.size() - 1; j++) 
            vars[j] = {*var_set.begin(), false}, var_set.erase(var_set.begin());
        
        auto constraint_comparator = [] (int j, int k) { return constraints[j].vars.size() < constraints[k].vars.size(); };
        for (int j = 1; j <= (int) vars.size() - 1; j++)
            sort(adj[vars[j].first].begin(), adj[vars[j].first].end(), constraint_comparator);
        auto var_comparator = [] (pair<int, bool> x, pair<int, bool> y) { return adj[x.first].size() > adj[y.first].size(); };
        sort(vars.begin() + 1, vars.end(), var_comparator);
        backtrack(i, 1);
    }
}

static vector<vector<int>> total_configs;
static vector<int> config;
static void make_total_configs(int index) {
    if (index == num_comps + 1) { total_configs.push_back(config); return; }
    for (vector<int> new_config : configs[index]) {
        config.insert(config.end(), new_config.begin(), new_config.end());
        make_total_configs(index + 1);
        config.erase(config.end() - new_config.size(), config.end());
    }
}
static void print_total_configs() {
    cout << "# CONFIGS: " << total_configs.size() << endl;
    for (int i = 0; i < (int) total_configs.size(); i++) {
        cout << "CONFIG " << i + 1 << ": ";
        for (int x : total_configs[i]){
            Square s = {x};
            cout << s.row << "," << s.col << " ";
        }
        cout << endl;
    }
}

static vector<long double> scores, score; // First is by #bombs left
static vector<long double> probs, prob; 
static void make_probs() {
    int num_squares = num_rows * num_cols;
    scores.resize(num_bombs + 1), score.resize(num_squares + 1);
    probs.resize(num_bombs + 1), prob.resize(num_squares + 1);

    int num_non_var_blanks = -total_vars.size();
    for (int i = 1; i <= num_squares; i++) num_non_var_blanks += (grid[i] == BLANK);
    int num_non_flag_bombs = num_bombs;
    for (int i = 1; i <= num_squares; i++) num_non_flag_bombs -= (grid[i] == FLAG);

    scores[0] = 1;
    for (int num_bombs_left = 1; num_bombs_left <= num_bombs; num_bombs_left++) 
        scores[num_bombs_left] = scores[num_bombs_left - 1] *
                                 (num_non_var_blanks - num_bombs_left + 1) /
                                 num_bombs_left;     
    
    long double score_sum = 0;
    for (vector<int> new_config : total_configs) { 
        int num_bombs_left = num_non_flag_bombs - new_config.size();
        if (num_bombs_left < 0) continue;
        long double new_score = scores[num_bombs_left];
        score_sum += new_score, probs[num_bombs_left] += new_score;
        for (int i : new_config) score[i] += new_score;
    }

    for (int num_bombs_left = 0; num_bombs_left <= num_bombs; num_bombs_left++) {
        probs[num_bombs_left] /= score_sum;
        for (int i = 1; i <= num_squares; i++) {
            if (grid[i] != BLANK) continue;
            if (total_vars.count(i)) continue;
            prob[i] += probs[num_bombs_left] * num_bombs_left / num_non_var_blanks;
        }
    }
    for (int i : total_vars) prob[i] = score[i] / score_sum; 
}
static void print_probs() {
    cout << fixed << setprecision(3);
    for (int i = 1; i <= num_rows; i++) {
        for (int j = 1; j <= num_cols; j++) {
            Square s = {i, j};
            cout << prob[s.index()] << " ";
        }
        cout << endl;
    }
}

static bool is_equal(long double x, long double y) {
    if (x > y) swap(x, y); 
    if (y == 0) return true;
    return (x / y >= 0.99);
}

Move do_probability() {
    make_graph();
    make_sets();
    // print_sets();
    make_configs();
    make_total_configs(1);
    // print_total_configs();
    make_probs();
    // print_probs();

    int num_squares = num_rows * num_cols;
    Move answer;
    for (int i = 1; i <= num_squares; i++) {
        if (grid[i] != BLANK) continue;
        if (is_equal(prob[i], 1)) answer = {true, {i}};
    }
    if (answer.is_valid()) return answer;

    long double min_prob = numeric_limits<long double>::infinity();
    for (int i = 1; i <= num_squares; i++) {
        if (grid[i] != BLANK) continue;
        min_prob = min(min_prob, prob[i]);
    }
    for (int i = 1; i <= num_squares; i++) {
        if (grid[i] != BLANK) continue;
        if (!is_equal(prob[i], min_prob)) continue;
        Square square = {i};
        if (square.is_corner()) answer = {false, {i}};
        else if (square.is_edge()) {
            if (!answer.is_valid() || !answer.square.is_corner()) answer = {false, {i}};
        } else { 
            if (!answer.is_valid()) answer = {false, {i}}; 
        }
    }
    return answer;
}

static void init() {
    num_rows = num_cols = num_bombs = num_comps = num_vars = num_cons = 0;
    grid.clear(), constraints.clear();
    adj.clear(), comp.clear(), constraint_sets.clear(), configs.clear();
    vars.clear(), total_vars.clear(), total_configs.clear(), config.clear();
    scores.clear(), score.clear(), probs.clear(), prob.clear();
    gauss_vars.clear(), gauss_vars_rev.clear(), gauss_cons.clear();
    mat.clear(), var_vals.clear();
}
pair<bool, pair<int, int>> next_move(int tmp_num_rows, int tmp_num_cols, int tmp_num_bombs, vector<vector<int>> tmp_grid, vector<vector<int>> tmp_weight, bool tmp_is_wrap) {
    init();
    num_rows = tmp_num_rows, num_cols = tmp_num_cols, num_bombs = tmp_num_bombs;
    int num_squares = num_rows * num_cols;
    grid.resize(num_squares + 1);
    for (int row = 1; row <= num_rows; row++) {
        for (int col = 1; col <= num_cols; col++) {
            Square square = {row, col};
            grid[square.index()] = tmp_grid[row][col];
        }
    }

    Move answer = do_logic();
    if (!answer.is_valid()) {
        make_constraints();
        answer = do_gauss();
        if (!answer.is_valid()) answer = do_probability();
    }
    assert(answer.is_valid());
    return {answer.is_flag, {answer.square.row, answer.square.col}};
}