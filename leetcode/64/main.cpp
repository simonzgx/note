//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>


using namespace std;

class Solution {
public:

    int minPathSum(vector<vector<int>> &grid) {
        if (grid.empty())return 0;
        if (grid[0].empty())return 0;
        int row = grid.size(), col = grid[0].size(), i, j;
        int **dp = static_cast<int **>(malloc(row * sizeof(int *)));
        for (i = 0; i < row; ++i) {
            *(dp + i) = static_cast<int *>(calloc(row, sizeof(int)));
        }
        dp[0][0] = grid[0][0];
        for (i = 1; i < row; ++i) {
            dp[i][0] = dp[i - 1][0] + grid[i][0];
        }
        for (i = 1; i < col; ++i) {
            dp[0][i] = dp[0][i - 1] + grid[0][i];
        }
        for (i = 1; i < row; ++i) {
            for (j = 1; j < col; ++j) {
                dp[i][j] = min(dp[i - 1][j], dp[i][j - 1]) + grid[i][j];
            }
        }
        int ret = dp[row - 1][col - 1];
        for (i = 0; i < row; ++i) {
            free(*(dp + i));
        }
        free(*dp);
        return ret;
    }
};

int main() {
    Solution s;
    vector<vector<int>> grid{
            {1, 2, 5},
            {3, 2, 1}
    };
    std::cout << s.minPathSum(grid);
}

