//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <memory>
#include <cstring>

using namespace std;

class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>> &obstacleGrid) {
        int **p = nullptr, i, j, row = obstacleGrid.size(), col = obstacleGrid[0].size();
        p = static_cast<int **>(malloc(sizeof(int *) * row));
        for (i = 0; i < obstacleGrid.size(); ++i) {
            *(p + i) = static_cast<int *>(malloc(sizeof(int) * col));
            std::memset(*(p + i), '\0', sizeof(int) * col);
        }
        for (i = 0; i < col; ++i) {
            if (obstacleGrid[0][i] == 1)break;
            p[0][i] = 1;
        }
        for (j = 0; j < row; ++j) {
            if (obstacleGrid[j][0] == 1)break;
            p[j][0] = 1;
        }
        for (i = 1; i < col; ++i) {
            for (j = 1; j < row; ++j) {
                if (obstacleGrid[j][i] == 1)continue;
                p[j][i] = p[j - 1][i] + p[j][i - 1];
            }
        }
        return p[row - 1][col - 1];
    }
};

int main() {
    Solution s;
    vector<vector<int>> obstacleGrid;
    obstacleGrid.emplace_back(std::vector<int>{0, 0, 0});
    obstacleGrid.emplace_back(std::vector<int>{0, 1, 0});
    obstacleGrid.emplace_back(std::vector<int>{0, 0, 0});
    cout << s.uniquePathsWithObstacles(obstacleGrid) << endl;
}