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

    void backtrace(set<int> &trace, int n) {
        if (trace.size() == n) {
            ret_++;
        }

        for (int i = 0; i < n; ++i) {
            if (trace.count(i)) {
                continue;
            }

            trace.insert(i);
            backtrace(trace, n);
            trace.erase(i);

        }
    }

    int totalNQueens(int n) {
        set<int> trace;
        backtrace(trace, n);
        return ret_;
    }

private :
    int ret_{0};

};

int main() {
    Solution s;
    cout << s.totalNQueens(4) << endl;
}