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
#include <memory>


using namespace std;

class Solution {
public:

    bool match(const char *s, const char *p, const char *s_end, const char *p_end) {
        if (p < p_end)return s < s_end;
        if (s < s_end) {
            if (p[0] == '*') {
                return match(s, p - 2, s_end, p_end);
            }
            return false;
        }

        char c = p[0];
        switch (c) {
            case '.': {
                return match(s - 1, p - 1, s_end, p_end);
            }
            case '*': {
                if (s[0] != *(p - 1) && *(p - 1) != '.')return match(s, p - 2, s_end, p_end);
                if (!match(s - 1, p, s_end, p_end)) {
                    return match(s, p - 2, s_end, p_end);
                }
                return true;
            }
            default: {
                if (s[0] == p[0] || p[0] == '.') {
                    return match(s - 1, p - 1, s_end, p_end);
                }
                return false;
            }
        }
    }

    bool isMatch(const string &s, const string &p) {
        return match(s.c_str() + s.size() - 1, p.c_str() + p.size() - 1, s.c_str(), p.c_str());
    }
};

int main() {
    Solution s;
    cout << s.isMatch("aaasdda", "aa.*") << endl;
}


