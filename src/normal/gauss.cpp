/*
Uses Gaussian elimination to determine whether the state of any hidden square can be 100% confirmed.
*/

#include "../../include/normal.h"
#include <bits/stdc++.h>
using namespace std;

static int num_vars, num_cons;
static vec<int> gauss_vars, gauss_vars_rev, gauss_cons;
static void gauss_setup() {
    int num_squares = get_num_squares();
    set<int> vars;
    gauss_cons.push_back(0); 
    for (int i = 1; i <= num_squares; i++) {
        const Constraint& cons = constraints[i];
        if (cons.is_empty()) continue;
        num_cons++;
        gauss_cons.push_back(i);
        for(const Square& square : cons.vars) 
            vars.insert(square.get_index());
    }

    num_vars = vars.size();
    gauss_vars.resize(num_vars + 1, 0), gauss_vars_rev.resize(num_squares + 1, 0);
    for (int i = 1; i <= num_vars; i++) {
        gauss_vars[i] = *vars.begin();
        gauss_vars_rev[gauss_vars[i]] = i;
        vars.erase(vars.begin());
    }
}

static vec<vec<ldoub>> mat; 
static void construct_augment() {
    mat.resize(num_cons + 2, vec<ldoub>(num_vars + 2, 0));

    for (int i = 1; i <= num_cons; i++) {
        const Constraint& cons = constraints[gauss_cons[i]];
        for(const Square& square : cons.vars) {
            mat[i][gauss_vars_rev[square.get_index()]] = 1;
        }
    }
    for (int i = 1; i <= num_cons; i++) {
        int index = gauss_cons[i];
        Square square = {index};
        mat[i][num_vars + 1] = grid[index].num_adj_bombs - square.get_num_adj_flags();
    }
}

static void gauss() {
    int r = num_cons, c = num_vars + 1;
    int j = 1;
    for (int curr = 1; curr <= r; curr++) {
        if (j > c) return;

        int i = curr;
        while (!mat[i][j]) {
            i++;
            if (i > r) {
                i = curr, j++;
                if (j > c) return;
            }
        }

        swap(mat[i], mat[curr]);
        int val = mat[curr][j];
        for (int id = 1; id <= c; id++) 
            mat[curr][id] /= val;

        for (int row = 1; row <= r; row++) {
            if (row == curr) continue;
            val = mat[row][j];
            for (int id = 1; id <= c; id++) 
                mat[row][id] -= val * mat[curr][id];
        }
        j++;
    }
}

static vec<int> var_vals;
static void backsub() {
    var_vals.resize(num_vars + 1, -1);
    for (int i = num_cons; i >= 1; i--) {
        int lb_value = 0, ub_value = 0;
        for (int j = 1; j <= num_vars; j++) {
            if (var_vals[j] != -1) {
                lb_value += var_vals[j] * mat[i][j];
                ub_value += var_vals[j] * mat[i][j];
            } else {
                if (mat[i][j] < 0) lb_value += mat[i][j];
                else ub_value += mat[i][j];
            }
        }

        if (mat[i][num_vars + 1] == lb_value) {
            for (int j = 1; j <= num_vars; j++) {
                if (mat[i][j] == 0 || var_vals[j] != -1) continue;
                var_vals[j] = (mat[i][j] < 0) ? 1 : 0;
            }
        } else if (mat[i][num_vars + 1] == ub_value) {
            for (int j = 1; j <= num_vars; j++) {
                if (mat[i][j] == 0 || var_vals[j] != -1) continue;
                var_vals[j] = (mat[i][j] < 0) ? 0 : 1;
            }
        }
    }
}

static void print_augment() {
    for (int i = 1; i <= num_cons; i++) {
        for (int j = 1; j <= num_vars + 1; j++) {
            cerr << mat[i][j] << " ";
        }
        cerr << "\n";
    }
}

static void init() {
    num_vars = num_cons = 0;
    gauss_vars.clear(), gauss_vars_rev.clear(), gauss_cons.clear();
    mat.clear(), var_vals.clear();
}

pair<bool, Move> do_gauss() {
    init();

    pair<bool, Move> ans = {false, {}};

    gauss_setup();
    construct_augment();
    gauss();
    backsub();

    for (int i = 1; i <= num_vars; i++) {
        if (var_vals[i] == -1) continue;
        Square square = {gauss_vars[i]};
        ans = {{true}, {(bool) var_vals[i], square}};
    }
    return ans;
}