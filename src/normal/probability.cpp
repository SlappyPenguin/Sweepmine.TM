/*
1) Connect all constraints which share variables (common squares) into graphical components

2) Recursively backtrack to find all solutions (variable configurations) to this CSP 
2.1) Optimise backtrack by fixing variables in decreasing order of adjacency size
2.2) Optimise backtrack by checking involved constraints by increasing number of variables (more likely to early 
exit) 
2.3) Optimise backtrack by early exiting when sum of the component is impossible to achieve

3) Assigns probabilities for every square being a bomb based on solutions
3.1) Weights solutions based on how many ways there are to arrange remaining bombs on the board
3.2) Squares which aren't variables get probabilities assigned based on if they could be a remaining bomb
3.3) Prefer uncovering corner over edge over non-edge squares
*/

#include "../../include/normal.h"
#include <bits/stdc++.h>
using namespace std;

static vec<vec<int>> adj;
static int num_comps;
static vec<int> comp;
static void dfs(int u) {
    comp[u] = num_comps;
    for (int v : adj[u]) {
        if (comp[v] != 0) continue;
        dfs(v);
    }
}
static void make_graph() {
    int num_squares = get_num_squares();
    adj.resize(num_squares + 1), comp.resize(num_squares + 1);
    for (int i = 1; i <= num_squares; i++) {
        const Constraint& cons = constraints[i];
        for (const Square& new_square : cons.vars) {
            int j = new_square.get_index();
            adj[i].push_back(j), adj[j].push_back(i);
        }
    }
        
    for (int i = 1; i <= num_squares; i++) {
        const Constraint& cons = constraints[i];
        if (cons.is_empty()) continue;
        if (comp[i] != 0) continue;
        num_comps++, dfs(i);
    }
}

static vec<vec<int>> constraint_sets; // Stores index 
static void make_sets() {
    int num_squares = get_num_squares();
    constraint_sets.resize(num_comps + 1);
    for (int i = 1; i <= num_squares; i++) {
        const Constraint& cons = constraints[i];
        if (cons.is_empty()) continue;
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

static vec<vec<vec<int>>> configs;
static vec<pair<int, bool>> vars; // Boolean stores: is this var set?
static hset<int> total_vars;
static void backtrack(int comp, int index) {
    if (index == (int) vars.size()) {
        vec<int> ans;
        for (const pair<int, bool>& var : vars) {
            if (!var.second) continue;
            ans.push_back(var.first);
        }
        configs[comp].push_back(ans);
        return;
    }

    int var = vars[index].first;
    for (bool value : {false, true}) {
        int halt_index = 0;
        for (int i : adj[var]) {
            Constraint& cons = constraints[i];
            cons.num_fixed++, cons.sum += value;
            int maximal_sum = cons.sum + cons.vars.size() - cons.num_fixed;

            if (cons.sum > cons.target_sum) halt_index = i;
            if (maximal_sum < cons.target_sum) halt_index = i;
            if (halt_index != 0) break;
        }

        if (halt_index != 0) {
            for (int i : adj[var]) {
                Constraint& cons = constraints[i];
                cons.num_fixed--, cons.sum -= value;
                if (i == halt_index) break;
            }
            continue;
        }

        vars[index].second = value;
        backtrack(comp, index + 1);
        for (int i : adj[var]) {
            Constraint& cons = constraints[i];
            cons.num_fixed--, cons.sum -= value;
        }
    }
}
static bool constraint_comp(int i, int j) {
    return constraints[i].vars.size() < constraints[j].vars.size();
}
static bool var_comp(pair<int, bool> x, pair<int, bool> y) {
    return adj[x.first].size() > adj[y.first].size();
}
static void make_configs() {
    configs.resize(num_comps + 1);
    for (int i = 1; i <= num_comps; i++) {
        hset<int> var_set;
        for (int j : constraint_sets[i]) {
            const Constraint& cons = constraints[j];
            for (const Square& square : cons.vars)
                var_set.insert(square.get_index());
        }          
        append_hset(total_vars, var_set);
        
        vars.clear(), vars.resize(var_set.size() + 1);
        for (int j = 1; j <= (int) vars.size() - 1; j++) {
            int index = *var_set.begin();
            var_set.erase(var_set.begin());
            vars[j] = {index, false};
        }
        
        for (int j = 1; j <= (int) vars.size() - 1; j++)
            sort(adj[vars[j].first].begin(), adj[vars[j].first].end(), constraint_comp);
        sort(vars.begin() + 1, vars.end(), var_comp);

        backtrack(i, 1);
    }
}

static vec<vec<int>> total_configs;
static vec<int> config;
static void make_total_configs(int index = 1) {
    if (index == num_comps + 1) { 
        total_configs.push_back(config); 
        return; 
    }
    for (const vec<int>& new_config : configs[index]) {
        append_vec(config, new_config);
        make_total_configs(index + 1);
        pop_vec(config, new_config);
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

// # ways that this # bombs could be placed on the non-variable squares
// = nCr(# blanks, # bombs)
static vec<ldoub> num_ways;
static vec<ldoub> prob_num_bombs, prob; 
static void make_probs() {
    int num_squares = get_num_squares();
    num_ways.resize(num_bombs + 1), prob_num_bombs.resize(num_bombs + 1), prob.resize(num_squares + 1);

    int num_non_var_blanks = -total_vars.size();
    for (int i = 1; i <= num_squares; i++) 
        num_non_var_blanks += (grid[i].state == State::Hidden);

    int num_non_flag_bombs = num_bombs;
    for (int i = 1; i <= num_squares; i++) 
        num_non_flag_bombs -= (grid[i].state == State::Flagged);

    num_ways[0] = 1;
    for (int num_bombs_left = 1; num_bombs_left <= num_bombs; num_bombs_left++) {
        num_ways[num_bombs_left] = num_ways[num_bombs_left - 1] *
                                  (num_non_var_blanks - num_bombs_left + 1) / num_bombs_left;     
    }
    
    ldoub total_num_ways = 0;
    for (const vec<int>& new_config : total_configs) { 
        int num_bombs_left = num_non_flag_bombs - new_config.size();
        if (num_bombs_left < 0) continue;

        ldoub new_num = num_ways[num_bombs_left];
        total_num_ways += new_num, prob_num_bombs[num_bombs_left] += new_num;
        for (int i : new_config) 
            prob[i] += new_num;
    }

    for (int i : total_vars) 
        prob[i] /= total_num_ways;
    for (int num_bombs_left = 0; num_bombs_left <= num_bombs; num_bombs_left++) {
        prob_num_bombs[num_bombs_left] /= total_num_ways;
        for (int i = 1; i <= num_squares; i++) {
            if (grid[i].state != State::Hidden) continue;
            if (total_vars.count(i)) continue;
            prob[i] += prob_num_bombs[num_bombs_left] * num_bombs_left / num_non_var_blanks;
        }
    }
}
static void print_probs() {
    cerr << fixed << setprecision(3);
    for (int i = 1; i <= num_rows; i++) {
        for (int j = 1; j <= num_cols; j++) {
            Square square = {i, j};
            cerr << prob[square.get_index()] << " ";
        }
        cerr << endl;
    }
}

static void init() {
    num_comps = 0;
    adj.clear(), comp.clear();
    constraint_sets.clear();
    configs.clear(), vars.clear(), total_vars.clear();
    total_configs.clear(), config.clear();
    num_ways.clear(), prob_num_bombs.clear(), prob.clear();
}

Move do_probability() {
    init();
    
    make_graph();
    make_sets();
    // print_sets();

    make_configs();
    make_total_configs();
    // print_total_configs();

    make_probs();
    // print_probs();    

    int num_squares = get_num_squares();
    pair<bool, Move> ans = {false, {}};
    for (int i = 1; i <= num_squares; i++) {
        if (grid[i].state != State::Hidden) continue;
        if (!is_equal(prob[i], 1)) continue;
        ans = {true, {true, {i}}};
    }
    if (ans.first) return ans.second;

    ldoub min_prob = INF;
    for (int i = 1; i <= num_squares; i++) {
        if (grid[i].state != State::Hidden) continue;
        min_prob = min(min_prob, prob[i]);
    }
    for (int i = 1; i <= num_squares; i++) {
        if (grid[i].state != State::Hidden) continue;
        if (!is_equal(prob[i], min_prob)) continue;

        Square square = {i};
        if (square.is_corner())  
            ans = {true, {false, {i}}};
        else if (square.is_edge() && (!ans.first || !ans.second.square.is_corner()))
            ans = {true, {false, {i}}};
        else if (!ans.first)
            ans = {true, {false, {i}}};
    }
    assert(ans.first);
    return ans.second;
}