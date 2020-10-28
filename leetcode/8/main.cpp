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

enum State {
    START = 0, NUM
};


class Solution {
public:
    int myAtoi(string s) {
        State st = START;
        long ans = 0;
        int sign = 1;
        for (const char &c : s) {
            switch (st) {
                case START:
                    if (c == ' ') {
                        break;
                    }
                    if (c == '-') {
                        sign = -1;
                        st = NUM;
                    } else if (c == '+') {
                        sign = 1;
                        st = NUM;
                    } else if (c >= '0' && c <= '9') {
                        st = NUM;
                        ans += sign*(c - '0');
                    } else {
                        return 0;
                    }
                    break;
                case NUM: {
                    if (c >= '0' && c <= '9') {
                        ans *= 10;
                        ans += sign * (c - '0');
                        if (ans > INT32_MAX)return INT32_MAX;
                        if (ans < INT32_MIN) return INT32_MIN;
                    } else {
                        return ans;
                    }
                }
            }
        }
        return ans;
    }
};

int main() {
    string str = "   -42333333333333";
    Solution s;
    cout << s.myAtoi(str) << endl;
}


