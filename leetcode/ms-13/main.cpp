#include <iostream>

using namespace std;


struct Move {
    int x;
    int y;
};

class Solution {
public:

    static int sum(int x) {
        int ret = 0;
        while (x) {
            ret += x % 10;
            x = x / 10;
        }
        return ret;
    }

    bool isValid(int x, int y) {
        return (0 <= x && x < _m) && (0 <= y && y < _n);
    }

    static int calcSum(int x, int y) {
        return sum(x) + sum(y);
    }

    void recursive(int x, int y, int k) {
        if (calcSum(x, y) <= k) {
            m_gird[x][y] = 1;
            _ret++;
        } else {
            m_gird[x][y] = -1;
            return;
        }
        for (const auto &n:m_move) {
            auto next_x = x + n.x;
            auto next_y = y + n.y;
            if (isValid(next_x, next_y) && m_gird[next_x][next_y] == 0) {
                recursive(next_x, next_y, k);
            }
        }
    }

    int movingCount(int m, int n, int k) {
        _m = m;
        _n = n;
        m_move[0] = {1, 0};
        m_move[1] = {0, 1};
        m_move[2] = {0, -1};
        m_move[3] = {-1, 0};
        m_gird = reinterpret_cast<int **>(malloc(sizeof(int *) * m));
        for (int i = 0; i < m; ++i) {
            m_gird[i] = static_cast<int *>(calloc(n, sizeof(int)));
        }
        recursive(0, 0, k);
        return _ret;
    }

private:
    int **m_gird;
    int _m{0}, _n{0},_ret{0};
    Move m_move[4];
};

int main() {
    Solution s;
    cout << s.movingCount(16, 8, 4) << endl;
}