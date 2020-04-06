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


    int minDistance(string word1, string word2) {
        int dp[word1.size() + 1][word2.size() + 1];
        int i, j;
        for (i = 0; i <= word1.size(); ++i) {
            for (j = 0; j <= word2.size(); ++j) {
                if (i == 0) {
                    dp[i][j] = j;
                } else if (j == 0) {
                    dp[i][j] = i;
                } else {
                    if (word1[i - 1] != word2[j - 1]) {
                        dp[i][j] = 1 + min(dp[i - 1][j - 1], min(dp[i - 1][j], dp[i][j - 1]));
                    } else {
                        dp[i][j] = 1 + min(dp[i - 1][j - 1] - 1, min(dp[i - 1][j], dp[i][j - 1]));
                    }
                }
            }
        }
        return dp[word1.size()][word2.size()];
    }

};

int main() {
    Solution s;
    string word1 = "horse";
    string word2 = "ros";
    cout << s.minDistance(word1, word2) << endl;
}