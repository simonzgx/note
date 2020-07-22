//
// Created by Administrator on 2020/6/30.
//
#include <stack>

using namespace std;


class CQueue {
public:
    CQueue() = default;

    void appendTail(int value) {
        push.push(value);
    }

    int deleteHead() {
        if (pop.empty() && push.empty())return -1;
        if (pop.empty() && !push.empty()) {
            while (!push.empty()) {
                pop.push(push.top());
                push.pop();
            }
        }
        int ret = pop.top();
        pop.pop();
        return ret;
    }

private :
    stack<int> push;
    stack<int> pop;
};