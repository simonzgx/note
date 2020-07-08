//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <unordered_map>


using namespace std;

class Solution {
public:
    vector<int> divingBoard(int shorter, int longer, int k) {
        if (k == 0) {
            return vector <int> ();
        }

        if (shorter == longer) {
            return vector <int> (1, shorter * k);
        }

        vector <int> lengths(k + 1);
        for (int i = 0; i <= k; ++i) {
            lengths[i] = shorter * (k - i) + longer * i;
        }

        return lengths;
    }
};

int main() {
    Solution s;
    auto r = s.divingBoard(1, 1, 100000);
    for (auto it:r) {
        cout << it << " ";
    }
    cout << endl;
}


