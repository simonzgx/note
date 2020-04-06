//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>

using namespace std;

class Solution {
public:

    bool isMatch(char c, string s) {
        if (s[0] == c || s[0] == '.')return true;
        return s.size() == 2 && s[0] == '.';
    }

    bool isMatch(string s, string p) {
        vector<string> elements;
        for (int i = 0; i < p.size() - 1; ++i) {
            if (p[i + 1] == '*') {
                elements.emplace_back(p.substr(i, 2));
            } else {
                elements.emplace_back(p.substr(i, 1));
            }
        }
        for(int i=0;i<elements.size();++i){
            for(int j=0;j<s.size();++j){
                if ()
            }
        }
    }
};

int main() {
    vector<int> nums{8, -19, 5, -4, 20};
    Solution s;
    string s1 = "abcd";
    cout << s1.substr(2, 2) << endl;
}