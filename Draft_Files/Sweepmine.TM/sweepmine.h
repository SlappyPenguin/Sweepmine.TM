#include<bits/stdc++.h>
using namespace std;

// Returns your next move - bool should be FALSE/0 for revealing a square, and TRUE/1 for toggling a flag.
pair<bool, pair<int, int>> next_move(int r, int c, int n, vector<vector<int>> grid, vector<vector<int>> charge, bool T);