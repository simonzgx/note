//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>

using namespace std;

int length;

class Solution {
public:
    int maxProfit(const vector<int> &prices) {
        if (prices.empty())return 0;
        int buy = -prices[0], sell = 0;
        for (int i = 1; i < prices.size(); ++i) {
            int tmp_buy = buy;
            buy = max(buy, sell - prices[i]);
            sell = max(tmp_buy + prices[i], sell);
        }
        return sell;

    }
};

int main() {
    Solution s;
    cout << s.maxProfit({7, 1, 5, 3, 6, 4});
}