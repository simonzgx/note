//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <set>

using namespace std;


class Solution {
public:
    int change(int amount, const vector<int> &coins) {
        int **dp = static_cast<int **>(malloc(sizeof(int *) * (coins.size() + 1)));
        int c, r;
        for (r = 0; r <= coins.size(); ++r) {
            *(dp + r) = static_cast<int *>(calloc(amount + 1, sizeof(int)));
        }
        for (r = 0; r <= coins.size(); ++r) {
            dp[r][0] = 1;
        }
        for (r = 1; r <= coins.size(); ++r) {
            for (c = 1; c <= amount; ++c) {
                if (c - coins[r - 1] >= 0) {
                    dp[r][c] = dp[r - 1][c] + dp[r][c - coins[r - 1]];
                } else {
                    dp[r][c] = dp[r - 1][c];
                }
            }
        }
        return dp[coins.size()][amount];
    }
};

int main() {
    Solution s;
    cout << s.change(5, std::vector<int>{1, 2, 5});
}