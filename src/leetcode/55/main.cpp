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
    bool canJump(vector<int> &nums) {
        if(nums.size() <= 1)return true;
        bool ans = false;
        int needStep = 1;
        for (auto it = nums.rbegin() + 1; it != nums.rend(); ++it) {
            if (*it >= needStep) {
                needStep = 1;
                ans = true;
                continue;
            } else {
                needStep++;
                ans = false;
            }
        }
        return ans;
    }
};

int main() {
    Solution s;
    std::vector<int> nums{3,2,1,0,4};
    cout<<s.canJump(nums)<<endl;
}