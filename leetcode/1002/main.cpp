#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
    using Index = std::unordered_map<char, int>;
public:

    static inline void filter(const string &s, Index &in, Index &out) {
        for (const char &c: s) {
            if (in[c] > 0) {
                in[c]--;
                out[c]++;
            }
        }
    }

    vector<string> commonChars(vector<string> &A) {
        vector<string> ret;
        if (A.empty()) {
            return ret;
        }

        Index ans;
        for (char &c : A[0]) {
            ans[c]++;
        }

        for (int i = 1; i < A.size(); ++i) {
            Index tmp;
            filter(A[i], ans, tmp);
            ans.swap(tmp);
        }

        for (auto &item : ans) {
            int times = item.second;
            while (times--) {
                ret.emplace_back(&item.first, 1);
            }
        }
        return ret;
    }

};

int main() {
    Solution s;
    vector<string> A{"cool","lock","cook"};
    auto ret = s.commonChars(A);
    for (const auto &item : ret) {
        cout << item << endl;
    }
    return 0;
}