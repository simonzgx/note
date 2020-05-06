//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <vector>
#include <stack>

using namespace std;



struct ListNode {
    int val;
    ListNode *next;

    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:

    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        std::stack<int> v1,v2;
        ListNode* n = l1;
        while(n){
            v1.push(n->val);
            n = n->next;
        }
        n = l2;
        while (n){
            v2.push(n->val);
            n = n->next;
        }
        int carry_flag = 0;
        ListNode* ans = nullptr;
        ListNode* cur = ans;
        while(!v1.empty() && !v2.empty()){
            ListNode tmp((carry_flag + v1.top() + v2.top())%10);
            *cur = tmp;
            cur = cur->next;
            carry_flag = (v1.top() + v2.top()) /10;
            v1.pop();
            v2.pop();
        }
        while(!v1.empty()){
            ListNode tmp((carry_flag + v1.top())%10);
            carry_flag = (carry_flag + v1.top())/10;
            *cur = tmp;
            cur=cur->next;
            v1.pop();
        }
        while(!v2.empty()){
            ListNode tmp((carry_flag + v2.top())%10);
            *cur = tmp;
            cur=cur->next;
            carry_flag = (carry_flag + v2.top())/10;
            v1.pop();
        }
        return ans;
    }
};

int main() {

}


