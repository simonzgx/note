//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <map>
#include <algorithm>
#include <functional>

using namespace std;

class Solution {
public:

    struct Helper {
        char c;
        int num;

        bool operator<(const Helper &lh) const {
            if (this->num < lh.num)return true;
            else if (this->num > lh.num)return false;
            return this->c < lh.c;
        }
    };

    string minWindow(string s, string t) {
        if (t.empty())return std::string();
        if (s.size() < t.size())return std::string();
        set<char> m;
        set<Helper> helpers;
        string ans;
        for (const auto &c:t) {
            m.insert(c);
            Helper tmp{c, 0};
            helpers.insert(tmp);
        }
        auto equal = [](const Helper &h, char c) -> bool {
            return h.c == c;
        };
        for (int left = 0, right = 0; right < s.size();) {
            if (m.count(s[right])) {
                auto it = find_if(helpers.begin(), helpers.end(), bind(equal, placeholders::_1, s[right]));
                Helper tmp{s[right], it->num + 1};
                helpers.erase(it);
                helpers.insert(tmp);
                if (helpers.begin()->num) {
                    while (!m.count(s[left]))++left;
                    if (ans.empty() || ans.size() > right - left + 1) {
                        ans = s.substr(left, right - left + 1);
                        if (ans.size() == t.size())return ans;
                    }
                }
            }
            while (helpers.begin()->num && left < right) {
                auto it = find_if(helpers.begin(), helpers.end(), bind(equal, placeholders::_1, s[left]));
                Helper tmp{s[left], it->num - 1};
                helpers.erase(it);
                helpers.insert(tmp);
                while (!m.count(s[++left]));
            }
            right++;
        }

        return ans;
    }
};

int main() {
    Solution s;
    cout << s.minWindow("ab", "b") << endl;
}