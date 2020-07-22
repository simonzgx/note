//
// Created by Administrator on 2020/6/30.
//
#include <stack>
#include <vector>

using namespace std;

class Solution {
public:
    int minArray(vector<int> &numbers) {
        if (numbers.size() == 1)return numbers[0];
        for (int i = 1; i < numbers.size(); ++i) {
            if (numbers[i - 1] > numbers[i]) {
                return numbers[i];
            }
        }
        return numbers[0];
    }
};

int main() {

}