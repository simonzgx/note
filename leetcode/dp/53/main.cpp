//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>

using namespace std;

class Solution {
public:
    static int crossSum(vector<int> nums, int left, int right, int p) {
        if (left == right)return nums[left];
        int sum = 0, leftSum = INT_MIN, rightSum = INT_MIN;
        for (int i = p; i >= left; --i) {
            sum += nums[i];
            leftSum = max(sum, leftSum);
        }
        sum = 0;
        for (int i = p + 1; i <= right; ++i) {
            sum += nums[i];
            rightSum = max(sum, rightSum);
        }
        return leftSum + rightSum;
    }

    int helpers(vector<int> nums, int left, int right) {
        if (left == right)return nums[left];
        int p = (left + right) / 2;
        int leftMax = helpers(nums, left, p);
        int rightMax = helpers(nums, p + 1, right);
        int crossMax = crossSum(nums, left, right, p);
        return max(leftMax, max(rightMax, crossMax));
    }

    int maxSubArray(vector<int> nums ) {
        return helpers(nums, 0, nums.size() - 1);
    }

};

int main() {
    vector<int> nums{8, -19, 5, -4, 20};
    Solution s;
    cout << s.maxSubArray(nums) << endl;
}