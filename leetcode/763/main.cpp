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
#include <memory>


using namespace std;

struct Helper {
    int first{};
    int second{};

    Helper(int f, int s) : first(f), second(s) {}
};


class Solution {
public:
    vector<int> partitionLabels(string S) {
        map<char, shared_ptr<Helper>> m;
        vector<shared_ptr<Helper>> vec;
        vector<int> ret;
        if (S.empty())return ret;
        int i, f, s;
        for (i = 0; i < S.size(); ++i) {
            auto it = m.find(S[i]);
            if (it != m.end()) {
                it->second->second = i;
            } else {
                auto tmp = std::make_shared<Helper>(i, i);
                vec.emplace_back(tmp);
                m.insert({S[i], tmp});
            }
        }

        i = 0;

        while (i < vec.size()) {
            s = vec[i]->second;
            f = vec[i]->first;
            while (i < vec.size() && (vec[i]->second < s || vec[i]->first <= s)) {
                if (vec[i]->second > s && vec[i]->first < s) {
                    s = vec[i]->second;
                }
                i++;
            }
            ret.push_back(s - f + 1);
        }
        return ret;
    }
};

int main() {
    Solution s;
    for (int i:s.partitionLabels("ababcbacadefegdehijhklij")) {
        cout << i << endl;
    }

}