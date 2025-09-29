#include "header.h"
#include <bits/stdc++.h>
using namespace std;

/*
static int num_vars, num_cons;
static vector<int> gauss_vars, gauss_vars_rev;
static vector<int> gauss_cons;
static vector<vector<long double>> mat;
static vector<int> var_vals;

need to be inited -> i.e. .clear() for vectors and = 0 for int
*/

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
        Square ds = {gauss_vars[i]};
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