//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <regex>


using namespace std;


class Solution {
public:
    static int partition(vector<int> &n, int low, int high) {
        int base = n[low];
        while (low < high) {
            while (high > low && n[high] >= base)high--;
            n[low] = n[high];
            while (high > low && n[low] <= base)low++;
            n[high] = n[low];
        }
        n[low] = base;
        return low;
    }

    static void quickSort(vector<int> &candidates, int low, int high) {
        if (low < high) {
            int p = partition(candidates, low, high);
            quickSort(candidates, low, p - 1);
            quickSort(candidates, p + 1, high);
        }
    }

    void find(const vector<int> &nums, int idx, int sum, vector<int> &path, int target) {
        if (sum == target) {
            for (int j = 0; j < path.size(); ++j) {
                int n = path[j];
                vector<int> tmp;
                while (n--) {
                    if (nums[j])tmp.push_back(nums[j]);
                }
                if (!tmp.empty())ans_.emplace_back(tmp);
            }
            return;
        }
        if (idx >= nums.size() || sum > target) {
            return;
        }
        for (int i = 0; i <= target / nums[idx]; ++i) {
            path.push_back(i);
            find(nums, idx + 1, sum + i * nums[idx], path, target);
            path.pop_back();
        }
    }

public:
    vector<vector<int>> combinationSum(vector<int> &candidates, int target) {
        quickSort(candidates, 0, candidates.size() - 1);
        vector<int> path;
        find(candidates, 0, 0, path, target);
        return ans_;
    }

private:
    vector<vector<int>> ans_;
};


int main() {
    Solution s;
    std::vector<int> nums{2, 3, 6, 7};
    auto ret = s.combinationSum(nums, 7);
    for (const auto &vec: ret) {
        for (const auto &it : vec) {
            cout << it << " ";
        }
        cout << endl;
    }
}