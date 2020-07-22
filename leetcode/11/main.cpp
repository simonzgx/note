//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>


using namespace std;

class Solution {
public:
    int maxArea(vector<int> &height) {
        int ans = 0;
        if (height.size() <= 1)return ans;
        int left = 0, right = height.size() - 1;
        while (true) {
            int tmp = (right - left) * min(height.at(left), height.at(right));
            ans = tmp > ans ? tmp : ans;
            if(right-left==1)break;
            if(height.at(left) < height.at(right))left++;
            else right--;
        }
        return ans;
    }
};

int main() {
    Solution s;
    std::vector<int> nums{1,3,2,5,25,24,5};
    cout << s.maxArea(nums) << endl;
}