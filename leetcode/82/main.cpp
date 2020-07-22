//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>

using namespace std;

class Solution {
public:
    char toLowerCase(char c) {
        static int diff = 'A' - 'a';
        if (('a' <= c && c <= 'z') || ('0' <= c && c <='9')) {
            return c;
        }
        if (('A' <= c && c <= 'Z')) {
            return static_cast<char>(c - diff);
        }
        return '\0';
    }

    bool isPalindrome(string s) {
        int left = 0, right = s.size() - 1;
        while (left < right) {
            char l, r;
            do {
                l = toLowerCase(s[left]);
                left++;
            } while (l == '\0' && left <= right);
            do {
                r = toLowerCase(s[right]);
                right--;
            } while (r == '\0' && left <= right + 1);
            if (l != r) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    Solution s;
    cout << s.isPalindrome("0P") << endl;
}


