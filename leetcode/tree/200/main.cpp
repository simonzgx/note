//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>

using namespace std;

struct Index {
    int col;
    int row;
};

class Solution {
public:

    void dfs(int c, int r, vector<vector<char>> &grid) {
        if (grid[c][r] == '1') {
            grid[c][r] = '0';
            for (auto &dir:dirs) {
                if (c + dir.col >= 0 && c + dir.col < _col && r + dir.row >= 0 && r + dir.row < _row) {
                    dfs(c + dir.col, r + dir.row, grid);
                }
            }
        }
    }

    int numIslands(vector<vector<char>> &grid) {
        int ans = 0;
        if (grid.empty())return ans;
        _col = grid.size(), _row = grid[0].size();
        int i, j;
        for (i = 0; i < _col; ++i) {
            for (j = 0; j < _row; ++j) {
                if (grid[i][j] == '1') {
                    dfs(i, j, grid);
                    ans++;
                }
            }
        }
        return ans;
    }

private:
    vector<Index> dirs{{1,  0},
                       {0,  -1},
                       {0,  1},
                       {-1, 0}};
    int _col, _row;
};

int main() {
    Solution s;
    vector<vector<char>> v;
    vector<char> t;
    for (char &c: string("11000")) {
        t.push_back(c);
    }
    v.emplace_back(t);
    t.clear();
    for (char &c: string("11000")) {
        t.push_back(c);
    }
    v.emplace_back(t);
    t.clear();
    for (char &c: string("00100")) {
        t.push_back(c);
    }
    v.emplace_back(t);
    t.clear();
    for (char &c: string("00011")) {
        t.push_back(c);
    }
    v.emplace_back(t);
    cout << s.numIslands(v) << endl;
}