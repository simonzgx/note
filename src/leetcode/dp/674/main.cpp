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
    int findLengthOfLCIS(vector<int> &nums) {
        if (nums.size() <= 1)return nums.size();
        int start = 0, i = 1;
        int ans = -1;
        for (; i < nums.size(); ++i) {
            if (nums[i] <= nums[i - 1]) {
                ans = max(ans, i - start);
                start = i;
            }
        }
        return max(ans, i - start);
    }
};

int main() {
    Solution s;
    vector<int> num{1, 3, 5, 7};
    cout << s.findLengthOfLCIS(num);
}