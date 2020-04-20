//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <algorithm>

using namespace std;


struct Element {
    int index;
    int value;

    friend bool operator<(const Element &left, const Element &right) {
        return left.value < right.value;
    }
};

class Solution {
public:
    int getMaxRepetitions(string s1, int n1, string s2, int n2) {
        int i = 0, j = 0;
        int cnt1 = 0, cnt2 = 0, min_match = 0;
        bool flag = true;
        while (true) {
            if (s1[i++] == s2[j])j++;
            if (i == s1.size()) {
                if (flag)min_match++;
                i = 0;
                cnt1++;
            }
            if (j == s2.size()) {
                flag = false;
                cnt2++;
                j = 0;
            }
            if (!j && !i)break;
        }
        int ans = n1 / cnt1 * cnt2 / n2;
        if (n1 % cnt1 > min_match)ans++;
        return ans;
    }

    vector<int> twoSum(vector<int> &nums, int target) {
        vector<Element> _nums;
        for(int i=0;i<nums.size();++i){
            Element e{i, nums[i]};
            _nums.emplace_back(e);
        }
        sort(_nums.begin(), _nums.end());
        for (int i = 0; i < _nums.size(); ++i) {
            for (int j = i + 1; j < nums.size(); ++j) {
                if (_nums[i].value + _nums[j].value == target)return {_nums[i].index, _nums[j].index};
            }
        }
        return {};
    }


};


int main() {
    Solution s;
    vector<int> nums{3, 2, 4};
    for (const auto &i : s.twoSum(nums, 6)) {
        cout << i << endl;
    }
}