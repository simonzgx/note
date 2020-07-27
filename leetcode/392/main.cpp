#include <string>
#include <iostream>
#include <unordered_map>

#include <vector>

#include <queue>

using namespace std;

class Solution {
public:
    bool isSubsequence(const string &s, const string &t) {
        int i = 0, j = 0;
        for (; j < t.size(); ++j) {
            if (t[j] == s[i]) {
                ++i;
            }
        }
        return i == s.size();
    }
};

int main() {
    Solution s;
    std::cout << s.isSubsequence("axc", "ahbgdc");
    return 0;
}