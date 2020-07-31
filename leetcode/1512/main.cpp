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
    int numIdenticalPairs(const vector<int> &nums) {
        std::map<int, int> memo;
        int ret = 0;
        for (int num : nums) {
            auto it = memo.find(num);
            if (it != memo.end()) {
                ret += it->second;
                it->second += 1;
            } else {
                memo[num] = 1;
            }
        }
        return ret;
    }
};

int main() {
    vector<int> arr{1,2,3,1,1,3};
    Solution s;
    cout << s.numIdenticalPairs(arr) << endl;
}


