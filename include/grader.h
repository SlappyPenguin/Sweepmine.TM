#include <bits/stdc++.h>
using namespace std;

// TODO: implement this function
pair<bool, pair<int, int>> next_move(
    int num_rows, int num_cols, int num_bombs, 
    vector<vector<int>> grid, vector<vector<int>> charge, 
    bool is_torus
);