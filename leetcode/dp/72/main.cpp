//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <map>


using namespace std;

class Solution {
public:

    int recursive(int i, int j) {
        if (i < 0) {
            return j+1;
        }
        if (j < 0) {
            return i+1;
        }
        if (memo.count({i, j})) {
            return memo[{i, j}];
        }
        int ret;
        if (word1[i] == word2[j]) {
            ret = recursive(i - 1, j - 1);
        } else {
            ret = min(min(recursive(i - 1, j - 1) + 1, recursive(i - 1, j) + 1), recursive(i, j - 1) + 1);
        }
        memo[{i, j}] = ret;
        return ret;
    }

    int minDistance(const string &w1, const string &w2) {
        int i = w1.size(), j = w2.size();
        if (i == 0)return j;
        if (j == 0)return i;
        this->word1 = w1;
        this->word2 = w2;
        recursive(i, j);
        return memo[{i, j}];
    }

private:
    string word1, word2;
    map<std::pair<int, int>, int> memo;
};

int main() {
    Solution s;
    string word1 = "horse";
    string word2 = "ros";
    cout << s.minDistance(word1, word2) << endl;
}