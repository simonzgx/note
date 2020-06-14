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
public:

    int findBestValue(vector<int> &arr, int target) {
        int size = arr.size();
        sort(arr.begin(), arr.end());
        int def = target / size;
        if ((def + 1) * size - target < target - def * size) {
            def += 1;
        }
        int sum = 0, diff = 0, i = 0;
        for (; i < size; ++i) {
            if (i == 0) {
                if (arr[i] * size > target)
                    return def;
                else {
                    diff = target - arr[i] * size;
                    sum = arr[i];
                }
                continue;
            }
            int cur_diff = target - sum - arr[i] * (size - i);
            if (cur_diff < 0) {
                def = (target - sum) / (size - i);
                if ((def + 1) * (size - i) - (target - sum)  < (target - sum) - def * (size - i)) {
                    return def + 1;
                }
                return def;
            }
            if (cur_diff > diff) {
                return arr[i - 1];
            }
            sum += arr[i];
            diff = cur_diff;
        }
        return arr[i - 1];
    }
};

int main() {
    vector<int> arr{1547, 83230, 57084, 93444, 70879};
    Solution s;
    cout << s.findBestValue(arr, 71237) << endl;
}


