#include <bits/stdc++.h>
using namespace std;
const int MAX_NUM_ROWS = 100, MAX_NUM_COLS = 100, MAX_NUM_BOUNDARIES = 1000, MAX_NUM_BOMBS = 1000;
const int UNDECIDED = -1, BOMB = -2, NO_BOMB = -3, ERROR = -404;

int num_rows, num_cols, num_undecided_bombs;
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
pair<int, int> find_info(int row, int col) { // {num_undecideds, num_bombs}
    assert(state[row][col] >= 0);
    int num_bombs = 0, num_undecideds = 0;
    for (pair<int, int> neighbour : neighbours(row, col)) {
        num_bombs += (bool) (state[neighbour.first][neighbour.second] == BOMB);
        num_undecideds += (bool) (state[neighbour.first][neighbour.second] == UNDECIDED);
    }
    return {num_undecideds, num_bombs};
}

pair<bool, pair<int, int>> check1(int row, int col) {
    if (state[row][col] < 0) return {false, {-1, -1}};
    pair<int, int> info = find_info(row, col);
    int num_undecideds = info.first, num_bombs = info.second;
    if (num_undecideds != state[row][col] - num_bombs) return {false, {-1, -1}};
    for (pair<int, int> neighbour : neighbours(row, col))
        if (state[neighbour.first][neighbour.second] == UNDECIDED) return {false, neighbour};
    return {false, {-1, -1}};
}
pair<bool, pair<int, int>> check2(int row, int col) {
    if (state[row][col] < 0) return {false, {-1, -1}};
    pair<int, int> info = find_info(row, col);
    int num_bombs = info.second;
    if (state[row][col] != num_bombs) return {false, {-1, -1}};
    for (pair<int, int> neighbour : neighbours(row, col))
        if (state[neighbour.first][neighbour.second] == UNDECIDED) return {true, neighbour}; 
    return {false, {-1, -1}};
}
pair<bool, pair<int, int>> do_logic() {
    for (int row = 1; row <= num_rows; row++) {
        for (int col = 1; col <= num_cols; col++) {
            pair<bool, pair<int, int>> answer = max(check1(row, col), check2(row, col));
            if (answer.second.first != -1) return answer;
        }
    }
    return {false, {-1, -1}};
}

bool is_boundary(int row, int col) {
    if (state[row][col] != -1) return false;
    for (pair<int, int> neighbour : neighbours(row, col)) 
        if (state[neighbour.first][neighbour.second] >= 0) return true;
    return false;
}
int num_boundaries, num_non_boundary_undecideds;
pair<int, int> boundary[MAX_NUM_BOUNDARIES];

bool is_impossible(int row, int col) {
    if (state[row][col] < 0) return false;
    pair<int, int> info = find_info(row, col);
    int num_undecideds = info.first, num_bombs = info.second;
    if (num_bombs > state[row][col]) return true;
    if (num_bombs < state[row][col] && num_undecideds == 0) return true;
    return false;
}
vector<vector<pair<int, int>>> configurations;
void backtrack(int index) {
    if (index == num_boundaries + 1) {
        vector<pair<int, int>> new_configuration;
        for (int index2 = 1; index2 <= num_boundaries; index2++)
            if (state[boundary[index2].first][boundary[index2].second] == BOMB) 
                new_configuration.push_back(boundary[index2]);
        configurations.push_back(new_configuration);
        return;
    }

    int row = boundary[index].first, col = boundary[index].second;
    for (int new_state : {BOMB, NO_BOMB}) {
        state[row][col] = new_state;
        pair<int, int> halt_cause = {-1, -1};
        for (pair<int, int> neighbour : neighbours(row, col))
            if (is_impossible(neighbour.first, neighbour.second)) halt_cause = neighbour;
        if (halt_cause.first == -1) backtrack(index + 1);
        state[row][col] = UNDECIDED;
    }
}

pair<int, int> find_extra_boundary() {
    pair<int, int> answer = {-1, -1};
    int min_num_neighbours = INT_MAX;
    for (int row = 1; row <= num_rows; row++) {
        for (int col = 1; col <= num_cols; col++) {
            if (state[row][col] != -1) continue;
            if (is_boundary(row, col)) continue;
            int num_neighbours = neighbours(row, col).size();
            if (num_neighbours >= min_num_neighbours) continue;
            min_num_neighbours = num_neighbours, answer = {row, col};
        }
    }
    return answer;
}
long double score_by_num_unused_bombs[MAX_NUM_BOMBS];
long double score[MAX_NUM_ROWS][MAX_NUM_COLS];
pair<bool, pair<int, int>> do_probabilities() {
    for (int row = 1; row <= num_rows; row++) {
        for (int col = 1; col <= num_cols; col++) {
            if (state[row][col] != -1) continue;
            if (is_boundary(row, col)) num_boundaries++, boundary[num_boundaries] = {row, col};
            else num_non_boundary_undecideds++;
        }
    }
    pair<int, int> extra_boundary = find_extra_boundary();
    if (extra_boundary.first != -1) {
        num_boundaries++, boundary[num_boundaries] = extra_boundary;
        num_non_boundary_undecideds--;
    }
    assert(num_boundaries != 0);
    
    backtrack(1);

    // This prints all configurations, even ones impossible due to #bombs. However, this is dealt with later.
    // for (int i = 0; i < configurations.size(); i++) {
    //     cout << "CONFIG " << i << ": " << endl;
    //     for (pair<int, int> x : configurations[i])
    //         cout << x.first << " " << x.second << endl;
    // }

    // I trust long doubles more than God
    score_by_num_unused_bombs[0] = 1;
    for (int num_unused_bombs = 1; num_unused_bombs <= num_undecided_bombs; num_unused_bombs++) {
        long double s = num_non_boundary_undecideds;
        int u = num_unused_bombs;
        score_by_num_unused_bombs[u] = score_by_num_unused_bombs[u - 1] * (s - u + 1) / u;
    }   

    long double sum_score = 0;
    for (vector<pair<int, int>> configuration : configurations) {
        int num_unused_bombs = num_undecided_bombs - configuration.size();
        if (num_unused_bombs < 0) continue;
        assert(num_undecided_bombs - num_unused_bombs >= 0);
        long double configuration_score = score_by_num_unused_bombs[num_unused_bombs];
        sum_score += configuration_score;
        for (pair<int, int> square : configuration) 
            score[square.first][square.second] += configuration_score;
    }
    
    pair<int, int> answer = {-1, -1};
    long double max_score = -1;
    for (int index = 1; index <= num_boundaries; index++) {
        int row = boundary[index].first, col = boundary[index].second;
        if (score[row][col] <= max_score) continue;
        max_score = score[row][col], answer = {row, col};
    }
    if (max_score / sum_score >= 0.99) return {false, answer};

    long double min_score = INFINITY;
    for (int index = 1; index <= num_boundaries; index++) {
        int row = boundary[index].first, col = boundary[index].second;
        if (score[row][col] >= min_score) continue;
        min_score = score[row][col], answer = {row, col};
    }
    return {true, answer};
}

void init() {
    for (int num_unused_bombs = 0; num_unused_bombs <= num_undecided_bombs; num_unused_bombs++) 
        score_by_num_unused_bombs[num_undecided_bombs] = ERROR;
    for (int row = 1; row <= num_rows; row++)
        for (int col = 1; col <= num_cols; col++)
            state[row][col] = ERROR, score[row][col] = ERROR;
    for (int index = 1; index <= num_boundaries; index++) boundary[index] = {-ERROR, -ERROR};
    configurations.clear();
    num_rows = num_cols = num_undecided_bombs = num_boundaries = num_non_boundary_undecideds = 0;
}
pair<bool, pair<int, int>> next_move(int tmp_num_rows, int tmp_num_cols, int tmp_num_undecided_bombs, vector<vector<int>> tmp_state) {
    init();
    num_rows = tmp_num_rows, num_cols = tmp_num_cols, num_undecided_bombs = tmp_num_undecided_bombs;
    for (int row = 1; row <= num_rows; row++) {
        for (int col = 1; col <= num_cols; col++) {
            state[row][col] = tmp_state[row][col];
            if (state[row][col] == -2) num_undecided_bombs--;
        }
    }

    pair<bool, pair<int, int>> answer = do_logic();
    if (answer.second.first != -1) return answer;
    answer = do_probabilities();
    return answer;
}
