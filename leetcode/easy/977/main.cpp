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
    vector<int> sortedSquares(vector<int> &A) {
        vector<int> ret, r1, r2;
        int i, j;
        for (i = 0; i < A.size(); ++i) {
            if (A[i] >= 0) {
                break;
            }
            r1.push_back(A[i] * A[i]);
        }

        for (; i < A.size(); ++i) {
            r2.push_back(A[i] * A[i]);
        }
        auto it1 = r1.rbegin();
        auto it2 = r2.begin();
        for (; it1 != r1.rend() && it2 != r2.end();) {
            if (*it1 < *it2) {
                ret.push_back(*it1);
                it1++;
            } else {
                ret.push_back(*it2);
                it2++;
            }
        }

        while (it1 != r1.rend())ret.push_back(*it1++);
        while (it2 != r2.end())ret.push_back(*it2++);

        return ret;
    }
};

int main() {
    Solution s;
    vector<int> t{-7, -3, 2, 3, 11};
    for (const auto &it : s.sortedSquares(t)) {
        cout << it << endl;
    }
}


