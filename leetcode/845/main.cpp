#include <iostream>
#include <vector>

enum Status {
    START = 0, HEAD, UP, DOWN
};


using namespace std;

class Solution {
public:
    int longestMountain(const vector<int> &A) {
        Status s = START;
        int start = 0, end = 0, i = 0;
        for (; i < A.size(); ++i) {
            switch (s) {
                case START:
                    start = i;
                    s = HEAD;
                    break;
                case HEAD:
                    if (A[i] > A[i - 1]) {
                        s = UP;
                    } else {
                        start = i;
                    }
                    break;
                case UP:
                    if (A[i] < A[i - 1]) {
                        s = DOWN;
                    } else if (A[i] == A[i - 1]) {
                        s = START;
                        start = i;
                    }
                    break;
                case DOWN:
                    if (A[i] > A[i - 1]) {
                        s = UP;
                        if (i - start > ans) {
                            ans = i - start;
                        }
                        start = i - 1;
                    } else if (A[i] == A[i - 1]) {
                        s = START;
                        if (i - start > ans) {
                            ans = i - start;
                        }
                        start = i;
                    }
                    break;
            }
        }
        if (s == DOWN && i - start > ans) {
            ans = i - start;
        }
        return ans;
    }

private:
    int ans{};
};

int main() {

    Solution s;
    cout << s.longestMountain({0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,1,0,1,0,1,0,0,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,0,0,0,1,0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,1,1,0,1,1,1,0,0,0,1,0,1,1}) << endl;

}