//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <set>
#include <cstdarg>

using namespace std;


class Solution {
public:
    int maxProfit(const vector<int> &prices) {
        if (prices.empty())return 0;
        int l01 = 0, l10 = -prices[0], l00 = 0, tmp10;
        for (int i = 1; i < prices.size(); ++i) {
            tmp10 = l10;
            l10 = l00 - prices[i] > l10 ? l00 - prices[i] : l10;
            l00 = l00 > l01 ? l00 : l01;
            l01 = tmp10 + prices[i];
        }
        return max(l00, l01);
    }
};

int main() {
    Solution s;
    cout << s.maxProfit(std::vector<int>{2, 1, 4});
}