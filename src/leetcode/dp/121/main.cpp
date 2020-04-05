//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>

using namespace  std;

int length;

int getValue(int i, int isHold, int sellTimes, int price, int dp[8][2][3]){
    if (isHold == 0 && sellTimes == 0)return 0;
    if(i == 0){
        if(isHold)return -price;
        else return INT_MIN;
    }
    if (sellTimes == 2){
        if(isHold) return INT_MIN;
        if(i<3)return INT_MIN;
    }
    if(isHold == 0 && sellTimes == 1)
        return max(dp[i-1][1][0]+price, dp[i-1][0][1]);
    if(isHold == 1 && sellTimes == 0)
        return max(dp[i-1][0][0]-price, dp[i-1][1][0]);
    if(isHold == 1 && sellTimes == 1)
        return max(dp[i-1][0][1]-price, dp[i-1][1][1]);
    if(isHold == 0 && sellTimes == 2)
        return max(dp[i-1][1][1]+price, dp[i-1][0][2]);
    return 0;

}

int maxProfit(vector<int>& prices) {
    int hold1 = INT_MIN, hold2 = INT_MIN;
    int release1 = 0, release2 = 0;
    for (const int&p : prices){
        release2 = max(release2, hold2+p);
        hold2 = max(hold2, release1 - p);
        release1 = max(release1, hold1+p);
        hold1=max(hold1,-p);
    }
    return release2;
}

int main(){
    vector<int> prices{3,3,5,0,0,3,1,4};
    cout<<maxProfit(prices)<<endl;
}