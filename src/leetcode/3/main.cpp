//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <functional>

using namespace std;


class Solution {
public:

    struct Helper {
        char v;
        int idx;

        bool operator<(const Helper &rhs) const {
            if (this->idx < rhs.idx)return true;
            else if (this->idx > rhs.idx) return false;

            return (this->v < rhs.v);
        }
    };

    int lengthOfLongestSubstring(string s) {
        if (s.empty())return 0;
        //char to index
        set<Helper> has;
        int ans = 0;
        int left, right;
        auto func = [](const Helper &e, char c) {
            return e.v == c;
        };
        for (left = 0, right = 0; right < s.size(); ++right) {
            function<bool(const Helper &e)> find = bind(func, placeholders::_1, s[right]);
            auto it = find_if(has.begin(), has.end(), find);
            if (it != has.end()) {
                ans = max(ans, right - left);
                left = it->idx + 1;
                has.erase(has.begin(), ++it);
            }
            Helper tmp{s[right], right};
            has.insert(tmp);
        }
        return max(ans, right - left);
    }
};

int main() {
    string str = "bbbbb";
    Solution s;
    cout << s.lengthOfLongestSubstring(str);
}


