//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

int trap(vector<int> &height) {
    if (height.empty())return 0;
    int sum = 0;
    vector<int> maxLeft, maxRight;
    for (int i = 0; i < height.size(); ++i) {
        if (i == 0) {
            maxLeft.push_back(0);
            continue;
        }
        maxLeft.push_back(max(maxLeft.at(maxLeft.size()-1), height.at(i - 1)));
    }
    for (int i = height.size(); i > 0; i--) {
        if (i == height.size()) {
            maxRight.push_back(0);
            continue;
        }
        maxRight.push_back(max(maxRight.at(maxRight.size()-1), height.at(i)));
    }
    for(int i=0;i<height.size();++i){
        int m =  min(maxLeft.at(i), maxRight.at(height.size()-i-1));
        if(height.at(i) < m){
            sum += m - height.at(i);
        }
    }

    return sum;
}

int main() {
    vector<int> input{2,0,2};
    cout << trap(input);
}
