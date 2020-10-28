//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_map>


using namespace std;

class Solution {
public:
    bool uniqueOccurrences(const vector<int> &arr) {
        unordered_map<int, int> m1, m2;
        for (const int &n : arr) {
            m1[n]++;
        }
        for (const auto &it:m1) {
            if (m2.count(it.second)) {
                return false;
            }
            m2[it.second]++;
        }
        return true;
    }
};

int main() {
    Solution s;
    cout << s.uniqueOccurrences({1, 2}) << endl;
}


