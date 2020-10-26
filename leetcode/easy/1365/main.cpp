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

    struct Helper {
        int val;
        int index;

        Helper(int v, int i) : val(v), index(i) {}

        bool operator<(const Helper &rh) const {
            return val < rh.val;
        }
    };

public:
    vector<int> smallerNumbersThanCurrent(vector<int> &nums) {
        vector<Helper> h;
        vector<int> ans = nums;
        h.reserve(nums.size());
        int i = 0;
        for (; i < nums.size(); ++i) {
            h.push_back({nums[i], i});
        }

        std::sort(h.begin(), h.end());
        ans[h[0].index] = 0;
        int tmp = 0;
        for (i = 1; i < h.size(); ++i) {
            if (h[i].val != h[i - 1].val) {
                tmp = i;
            }
            ans[h[i].index] = tmp;
        }
        return ans;
    }
};

int main() {
    Solution s;
    vector<int> t{8, 1, 2, 2, 3};
    for (const auto &it : s.smallerNumbersThanCurrent(t)) {
        cout << it << endl;
    }
}


