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

    static bool isCover(const unordered_map<char, int> &s1, unordered_map<char, int> &s2) {
        for (const auto &p:s1) {
            if (s2[p.first] != p.second) {
                return false;
            }
        }
        return true;
    }

    bool checkInclusion(string s1, string s2) {
        unordered_map<char, int> m_1, m_2;
        for (const auto &c:s1) {
            m_1[c]++;
        }
        for (int left = 0, right = 0; right < s2.size();) {
            if (m_1.count(s2[right])) {
                m_2[s2[right]]++;
                if (right - left + 1 == s1.size()) {
                    if (isCover(m_1, m_2)) {
                        return true;
                    } else {
                        m_2[s2[left]]--;
                        ++left;
                    }
                }
                ++right;
            } else {
                m_2.clear();
                ++right;
                left = right;
            }
        }
        return false;
    }
};

int main() {
    Solution s;
    cout << s.checkInclusion("xuumbjffxuzovdwrnolopeingppzgorotzdqfprokkmucxwsub",
                             "qiffktonvtrkcogosfbcvcosktnvbeitxuqpexxejnklndzbtz")
         << endl;
}