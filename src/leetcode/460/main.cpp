//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>


using namespace std;

class Solution {
public:
    int getMaxRepetitions(string s1, int n1, string s2, int n2) {
        int i = 0, j = 0;
        int cnt1 = 0, cnt2 = 0, min_match = 0;
        bool flag = true;
        while (true) {
            if (s1[i++] == s2[j])j++;
            if (i == s1.size()) {
                if (flag)min_match++;
                i = 0;
                cnt1++;
            }
            if (j == s2.size()) {
                flag = false;
                cnt2++;
                j = 0;
            }
            if (!j && !i)break;
        }
        int ans = n1 / cnt1 * cnt2/n2;
        if (n1 % cnt1 > min_match)ans++;
        return ans;
    }
};


int main() {
    Solution s;
    cout << s.getMaxRepetitions("bacab", 3, "abacab", 1) << endl;
}