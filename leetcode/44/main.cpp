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


    bool isMatch(const string &s, const string &p) {
        char **dp = static_cast<char **>(malloc(s.size() * (sizeof(char *))));
        int i, j;
        for (i = 0; i < s.size(); ++i) {
            *(dp + i) = static_cast<char *>(malloc(p.size()));
        }


    }
};

int main() {
    Solution s;
    cout << s.isMatch("cbadfddasadd", "c*adf*a") << endl;
}


