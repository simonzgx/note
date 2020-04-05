//
// Created by Simon on 2020/4/4.
//

#include <iostream>
#include <vector>

using namespace  std;

int maxProfit(vector<int>& prices) {
    int l = prices.size();
    if (l <= 1)return 0;
    int minPrice = prices.at(0);
    int ret = 0,diff;
    for(int i=1;i<l;++i){
        diff = prices[i] - minPrice;
        if (diff < 0)minPrice = prices[i];
        else if (diff > ret) ret = diff;
    }
    return ret;
}

int main(){
    vector<int> prices{7,1,5,3,6,4};
    cout<<maxProfit(prices)<<endl;
}