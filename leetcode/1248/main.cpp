#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int numberOfSubarrays(vector<int> &nums, int k) {
        int ans = 0;
        if (nums.size() < k) return ans;
        vector<int> index;
        index.emplace_back(-1);
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] & 1)index.emplace_back(i);
        }
        int length = nums.size();
        index.emplace_back(length);
        if(index.size() -1 < k)return ans;
        for (int i = 1; i < index.size(); ++i) {
            int j = i + k - 1;
            if (j >= index.size()-1) {
                break;
            }
            ans += (index[i]-index[i-1])*(index[j+1]-index[j]);
        }

        return ans;
    }
};

int main() {
    Solution s;
    vector<int> nums{2,2,2,1,2,2,1,2,2,2};
    cout << s.numberOfSubarrays(nums, 2) << endl;
    return 0;
}