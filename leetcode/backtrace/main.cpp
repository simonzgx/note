#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>


using namespace std;

vector<vector<int>> ans;

void backtrace(const vector<int> &nums, vector<int> &trace) {
    if (trace.size() == nums.size()) {
        ans.emplace_back(trace);
        return;
    }
    for (int i = 0; i < nums.size(); ++i) {
        auto it = find(trace.begin(), trace.end(), nums[i]);

        if (it != trace.end()) {
            continue;
        }
        trace.emplace_back(nums[i]);
        backtrace(nums, trace);
        trace.pop_back();
    }
}

int binSearch(const vector<int> &nums, int l, int r, int num) {
    if (l == r) {
        if (nums[l] == num) {
            return l;
        } else {
            return -1;
        }
    }
    int mid = (l + r) / 2;
    if (nums[mid] == num) {
        return mid;
    } else if (nums[mid] > num) {
        return binSearch(nums, l, mid, num);
    } else if (nums[mid] < num) {
        return binSearch(nums, mid + 1, r, num);
    }
}

int binSearch(const vector<int> &nums, int num) {
    int l = -1;
    int r = nums.size();
    int mid;
    while (r - l > 1) {
        mid = (r + l) / 2;
        if (nums[mid] == num) {
            return mid;
        } else if (nums[mid] > num) {
            r = mid;
        } else if (nums[mid] < num) {
            l = mid;
        }
    }
    return -1;
}

int main() {
    vector<int> nums;
    for (int i = 0; i < 10; ++i) {
        int n = rand() % 100;
        cout << n << " ";
        nums.emplace_back(n);
    }
    nums.emplace_back(3);
    cout << "\n";
    sort(nums.begin(), nums.end());
    cout << binSearch(nums, 3);

    return 0;
}