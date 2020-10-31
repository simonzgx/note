//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;


class Solution {
public:
    int islandPerimeter(vector<vector<int>> &grid) {
        if (grid.empty())return 0;
        int num1 = 0, num2 = 0;
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j] == 1) {
                    num2++;
                    if (i < grid.size() - 1 && grid[i + 1][j] == 1)num1++;
                    if (j < grid[i].size() - 1 && grid[i][j + 1] == 1)num1++;
                }
            }
        }
        return num2 * 4 - num1*2;
    }
};

int main() {
    vector<vector<int>> arr{{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}};
    Solution s;
    cout << s.islandPerimeter(arr) << endl;
}


