//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <set>

using namespace std;


struct Helper {
    int length;
    int idx;

    bool operator<(const Helper &rhs) const {
        if (this->length > rhs.length) {
            return true;
        } else if (this->length < rhs.length) {
            return false;
        }
        return this->idx > rhs.idx;
    }

    Helper(int l, int i) : length(l), idx(i) {}

    Helper() = default;
};

class Solution {
public:
    int lengthOfLIS(vector<int> &nums) {
        if (nums.size() <= 1)return nums.size();
        std::set<Helper> dp;
        dp.insert({1, 0});
        for (int i = 1; i < nums.size(); ++i) {
            auto it = dp.begin();
            Helper tmp{1, i};
            for (; it != dp.end(); ++it) {
                if (nums[i] > nums[it->idx]) {
                    tmp.length = it->length + 1;
                    tmp.idx = i;
                    break;
                }
            }
            dp.insert(tmp);
        }
        return dp.begin()->length;
    }
};

int main() {
    Solution s;
    vector<int> num{10, 9, 2, 5, 3, 7, 101, 18};
    cout << s.findLengthOfLCIS(num);
}