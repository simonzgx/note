#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct ValueWithIndex {
    int value;
    int index;

    bool operator<(const ValueWithIndex &lv) const {
        return this->value < lv.value;
    }

    ValueWithIndex(int v, int i) : value(v), index(i) {}
};


class Solution {
public:
    int largestRectangleArea(vector<int> &heights) {
        if (heights.empty())return 0;
        vector<ValueWithIndex> helper;
        int i, j, ans = 0;
        for (i = 0; i < heights.size(); ++i) {
            ValueWithIndex tmp{heights[i], i};
            helper.emplace_back(tmp);
        }
        sort(helper.begin(), helper.end());
        for (i = 0; i < helper.size(); ++i) {
            int height = helper.at(i).value;
            int number = 1;
            j = helper.at(i).index + 1;
            while (j < heights.size()) {
                if (heights.at(j) < height)break;
                number++;
                j++;
            }
            j = helper.at(i).index - 1;
            while (j >= 0) {
                if (heights.at(j) < height)break;
                j--;
                number++;
            }
            ans = max(number * height, ans);
        }
        return ans;
    }
};


int main() {
    vector<int> height{2, 1, 5, 6, 2, 3};
    Solution s;
    cout << s.largestRectangleArea(height) << endl;
    return 0;
}