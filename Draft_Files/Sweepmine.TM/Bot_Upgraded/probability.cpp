#include "header.h"
#include <bits/stdc++.h>
using namespace std;

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
        bool halt = false;
        for (int con : adj[var]) {
            Square square = {con};
            constraints[con].num_fixed++, constraints[con].sum += value;
            if (constraints[con].sum > grid[con] - square.num_adj_flags()) halt = true;
            if (constraints[con].sum + (int) constraints[con].vars.size() - constraints[con].num_fixed < grid[con] - square.num_adj_flags()) halt = true;
        }
        vars[index].second = value;
        if (!halt) backtrack(comp, index + 1);
        for (int con : adj[var]) 
            constraints[con].num_fixed--, constraints[con].sum -= value;
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