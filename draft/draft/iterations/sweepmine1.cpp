#include "bigint.h"
#include <bits/stdc++.h>
using namespace std;
const int MAX_NUM_ROWS = 100, MAX_NUM_COLS = 100, MAX_NUM_BOUNDARIES = 1000;
const int UNKNOWN_UNDECIDED = -1, UNKNOWN_BOMB = -2, UNKNOWN_NO_BOMB = -3;

int num_rows, num_cols, num_bombs;
int state[MAX_NUM_ROWS][MAX_NUM_COLS]; 
vector<pair<int, int>> neighbours(int row, int col) {
    vector<pair<int, int>> answer;
    for (int delta_row = -1; delta_row <= 1; delta_row++) {
        for (int delta_col = -1; delta_col <= 1; delta_col++) {
            if (delta_row == 0 && delta_col == 0) continue;
            int new_row = row + delta_row, new_col = col + delta_col;
            if (new_row < 1 || new_row > num_rows || new_col < 1 || new_col > num_cols) continue;
            answer.push_back({new_row, new_col});
        }
    }
    return answer;
}

bool is_boundary(int row, int col) {
    if (state[row][col] != -1) return false;
    for (pair<int, int> neighbour : neighbours(row, col)) 
        if (state[neighbour.first][neighbour.second] != -1) return true;
    return false;
}
int num_boundaries;
int num_non_boundary_unknowns;
pair<int, int> boundary[MAX_NUM_BOUNDARIES];

bool is_impossible(int row, int col) {
    if (state[row][col] < 0) return false;
    int num_bombs = 0, num_undecided = 0;
    for (pair<int, int> neighbour : neighbours(row, col)) {
        num_bombs += (bool) (state[neighbour.first][neighbour.second] == UNKNOWN_BOMB);
        num_undecided += (bool) (state[neighbour.first][neighbour.second] == UNKNOWN_UNDECIDED);
    }
    if (num_bombs > state[row][col]) return true;
    if (num_bombs < state[row][col] && num_undecided == 0) return true;
    return false;
}
vector<vector<pair<int, int>>> configurations;
void backtrack(int index) {
    if (index == num_boundaries + 1) {
        vector<pair<int, int>> new_configuration;
        for (int index2 = 1; index2 <= num_boundaries; index2++)
            if (state[boundary[index2].first][boundary[index2].second] == UNKNOWN_BOMB) 
                new_configuration.push_back(boundary[index2]);
        configurations.push_back(new_configuration);
        return;
    }

    int row = boundary[index].first, col = boundary[index].second;
    for (int new_state : {UNKNOWN_BOMB, UNKNOWN_NO_BOMB}) {
        state[row][col] = new_state;
        pair<int, int> halt_cause = {-1, -1};
        for (pair<int, int> neighbour : neighbours(row, col))
            if (is_impossible(neighbour.first, neighbour.second)) halt_cause = neighbour;
        if (halt_cause.first == -1) backtrack(index + 1);
        state[row][col] = UNKNOWN_UNDECIDED;
    }
}

bigint score[MAX_NUM_ROWS][MAX_NUM_COLS];
pair<bool, pair<int, int>> do_probabilities() {
    for (int row = 1; row <= num_rows; row++) {
        for (int col = 1; col <= num_cols; col++) {
            if (is_boundary(row, col)) 
                num_boundaries++, boundary[num_boundaries] = {row, col};
            else if (state[row][col] == -1) num_non_boundary_unknowns++;
        }
    }
    assert(num_boundaries != 0);
    
    backtrack(1);
    
    for (vector<pair<int, int>> configuration : configurations) {
        bigint num_unused_bombs = to_bigint(num_bombs) - to_bigint((int) configuration.size());
        if (num_unused_bombs < 0) continue;
        bigint num_non_boundary_unknowns2 = to_bigint(num_non_boundary_unknowns);

        bigint difference = num_non_boundary_unknowns2 - num_unused_bombs;
        if (difference < 0) continue;
        bigint configuration_score = big_fact(num_non_boundary_unknowns2) / (big_fact(num_unused_bombs) * big_fact(difference));

        for (pair<int, int> square : configuration) 
            score[square.first][square.second] += configuration_score;
    }
    pair<int, int> answer = {-1, -1};
    bigint min_score = -1;
    for (int index = 1; index <= num_boundaries; index++) {
        int row = boundary[index].first, col = boundary[index].second;
        if (min_score != -1 && score[row][col] >= min_score) continue;
        min_score = score[row][col];
        answer = {row, col};
    }
    return {true, answer};
}

int main() {
    // freopen("sweepmine.in", "r", stdin);
    // freopen("sweepmine.out", "w", stdout);

    cin >> num_rows >> num_cols >> num_bombs;
    for (int row = 1; row <= num_rows; row++)
        for (int col = 1; col <= num_cols; col++)
            cin >> state[row][col];
    
    pair<bool, pair<int, int>> answer = do_probabilities();
    cout << answer.first << '\n';
    cout << answer.second.first << " " << answer.second.second << '\n';
    

    // for (int i = 0; i < configurations.size(); i++) {
    //     cout << "CONFIG " << i << ": " << endl;
    //     for (pair<int, int> x : configurations[i])
    //         cout << x.first << " " << x.second << endl;
    // }
}

