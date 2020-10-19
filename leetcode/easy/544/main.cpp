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
    bool backspaceCompare(string S, string T) {
        stack<char> s, t;
        for (char &i : S) {
            if (i == '#') {
                if(!s.empty()){
                    s.pop();
                }
            } else {
                s.push(i);
            }
        }
        for (char &i : T) {
            if (i == '#') {
                if (!t.empty()) {
                    t.pop();
                }
            } else {
                t.push(i);
            }
        }

        if (s.size() != t.size())return false;
        while (!s.empty()) {
            if (s.top() != t.top()) {
                return false;
            }
            s.pop();
            t.pop();
        }
        return true;
    }
};

int main() {
    Solution s;
    cout << s.backspaceCompare("ab#c", "ad#c") << endl;
}


