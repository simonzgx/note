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
    struct Helper {
        char c;
        int num;

        bool operator<(const Helper &lh) const {
            if (this->num < lh.num)return true;
            else if (this->num > lh.num)return false;
            return this->c < lh.c;
        }
    };

    vector<int> findAnagrams(string s, string t) {
        unordered_map<char, int> need, window;
        for (char c : t) need[c]++;
        int left = 0, right = 0;
        int valid = 0;
        vector<int> res; // 记录结果
        while (right < s.size()) {
            char c = s[right];
            right++;
// 进⾏窗⼝内数据的⼀系列更新
            if (need.count(c)) {
                window[c]++;
                if (window[c] == need[c])
                    valid++;
            } else {
                left = right;
                valid = 0;
                window.clear();
                continue;
            }
// 判断左侧窗⼝是否要收缩
            while (right - left >= t.size()) {
// 当窗⼝符合条件时，把起始索引加⼊ res
                if (valid == need.size())
                    res.push_back(left);
                char d = s[left];
                left++;
// 进⾏窗⼝内数据的⼀系列更新
                if (need.count(d)) {
                    if (window[d] == need[d])
                        valid--;
                    window[d]--;
                }
            }
        }
        return res;
    }
};

int main() {
    Solution s;

}