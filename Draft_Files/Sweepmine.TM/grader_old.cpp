#include "sweepmine.h"
#include <bits/stdc++.h>
using namespace std;

static const int MAXR = 105, MAXC = 105, SEED_VAL = 0;
static int r, c, n, t, O, D, state[MAXR][MAXC], is_flag[MAXR][MAXC], is_shown[MAXR][MAXC];
static vector<vector<int>> charge;
// 0-9 numbers, -1 not shown, -2 flag
static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
static ofstream ouf("sweepmine.out");

static bool in(int x, int y) { return (x >= 1 && x <= r && y >= 1 && y <= c); }
static vector<pair<int, int>> get_neighbours(int x, int y) {
    int dx[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    int dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

    vector<pair<int, int>> ret;
    for(int i = 0; i < 8; i++) {
        if(t) ret.push_back({(x + dx[i] - 1 + r) % r + 1, (y + dy[i] - 1 + c) % c + 1});
        else if(in(x + dx[i], y + dy[i])) ret.push_back({x + dx[i], y + dy[i]});
    }
    return ret;
}

static void generate_grid(int x, int y) {
    int cnt = n;
    while(cnt > 0) {
        int rx = uniform_int_distribution<>(1, r)(rng);
        int ry = uniform_int_distribution<>(1, c)(rng);
        if(state[rx][ry] == -1 || (rx == x && ry == y)) continue;
        cnt--, state[rx][ry] = -1;
    }

    pair<int, int> bounds = {1, 1};
    if(O) bounds = {-1000, 1000};
    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            charge[i][j] = uniform_int_distribution<>(bounds.first, bounds.second)(rng);
            while(charge[i][j] == 0) charge[i][j] = uniform_int_distribution<>(bounds.first, bounds.second)(rng);
        }
    }

    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            if(state[i][j] == -1) continue;
            for(pair<int, int> neigh : get_neighbours(i, j)) state[i][j] += charge[neigh.first][neigh.second] * (state[neigh.first][neigh.second] == -1 ? 1 : 0);
        }
    }
}

static vector<vector<int>> vectorise_grid() {
    vector<vector<int>> ret(r+1, vector<int>(c+1, -1));
    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            if(is_flag[i][j]) {
                assert(!is_shown[i][j]);
                ret[i][j] = -2;
            } else if(!is_shown[i][j]) ret[i][j] = -1;
            else ret[i][j] = state[i][j];
        }
    }
    return ret;
}

static int vis[MAXR][MAXC];
static void r_dfs(int x, int y) {
    if(is_flag[x][y]) return;
    vis[x][y] = 1;
    if(state[x][y] != 0) return;
    for(auto &[i, j] : get_neighbours(x, y)) if(!vis[i][j]) r_dfs(i, j);
}
static void reveal_tile(int x, int y) {
    for(int i = 1; i <= r; i++) for(int j = 1; j <= c; j++) vis[i][j] = 0;
    r_dfs(x, y);
    for(int i = 1; i <= r; i++) for(int j = 1; j <= c; j++) if(vis[i][j]) is_shown[i][j] = 1;
}

static bool check_win() {
    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            if(state[i][j] == -1) continue;
            if(!is_shown[i][j]) return false;
        }
    }
    ouf << "WIN\n";
    cout << "You win.\n";
    return true;
}

static void print_grid() {
    vector<vector<int>> o_grid = vectorise_grid();
    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            ouf << o_grid[i][j] << " ";
        }
        ouf << "\n";
    }
    ouf.flush();
}

int main() {
    cin >> r >> c >> n >> t >> O >> D;
    charge.resize(r+1, vector<int>(c+1, 1));
    pair<bool, pair<int, int>> move;
    if(!D) {
        move = next_move(r, c, n, vectorise_grid(), charge, (bool)t);
        generate_grid(move.second.first, move.second.second);
    } else {
        for(int i = 1; i <= r; i++) {
            for(int j = 1; j <= c; j++) {
                cin >> state[i][j];
            }
        }
        for(int i = 1; i <= r; i++) {
            for(int j = 1; j <= c; j++) {
                cin >> charge[i][j];
                if(!O) assert(charge[i][j] == 1);
            }
        }
    }

    int ouf_pos_save = 0;
    ouf << r << " " << c << " " << n << " " << t << " " << D << "\n";
    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            ouf << state[i][j] << " ";
        }
        ouf << "\n";
    }
    for(int i = 1; i <= r; i++) {
        for(int j = 1; j <= c; j++) {
            ouf << charge[i][j] << " ";
        }
        ouf << "\n";
    }
    ouf.flush();

    if(!D) {
        if(move.first == false) reveal_tile(move.second.first, move.second.second);
        else is_flag[move.second.first][move.second.second] = !is_flag[move.second.first][move.second.second];
        ouf << move.first << " " << move.second.first << " " << move.second.second << "\n";
        if(check_win()) exit(0);
    }

    while(true) {
        move = next_move(r, c, n, vectorise_grid(), charge, (bool)t);
        ouf << move.first << " " << move.second.first << " " << move.second.second << "\n";
        if(state[move.second.first][move.second.second] == -1 && move.first == false) {
            ouf << "DIE\n";
            cout << "You died.\n";
            break;
        }
        if(move.first == false) reveal_tile(move.second.first, move.second.second);
        else is_flag[move.second.first][move.second.second] = !is_flag[move.second.first][move.second.second];
        if(D) print_grid();
        if(check_win()) break;
    }
}