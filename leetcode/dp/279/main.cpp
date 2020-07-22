//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

int numSquares(int n) {
    int m = static_cast<int>(sqrt(n)) + 1;
    vector<int> square,ret;
    for(int i=1;i<=m;i++){
        square.push_back(i*i);
    }
    for(int i=0;i<=n;++i){
        if (i==0){
            ret.push_back(0);
            continue;
        }
        int min=i,j=0;
        for(;j<square.size();++j){
            if(square.at(j) > i)break;
            if (ret.at(i-square.at(j))<min)min=ret.at(i-square.at(j));
        }
        ret.push_back(min+1);
    }

    return ret.at(ret.size()-1);
}

int main() {
    cout<<numSquares(13)<<endl;
}


