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
        ListNode vHead(0), *p = &vHead;
        int flag = 0;
        while (l1 || l2 || flag) {
            int tmp = 0;
            if (l1 != nullptr) tmp += l1->val;
            if (l2 != nullptr) tmp += l2->val;
            tmp += flag;

            flag = tmp / 10;
            tmp %= 10;

            ListNode *next = l1 ? l1 : l2;
            if (next == nullptr) next = new ListNode(tmp);
            next->val = tmp;

            p->next = next;
            p = p->next;
            l1 = l1 ? l1->next : nullptr;
            l2 = l2 ? l2->next : nullptr;
        }
        return vHead.next;
    }
};

ListNode *VectorToList(const vector<int> &v) {
    ListNode *ret, *cur;
    ret = new ListNode(0);
    cur = ret;
    for (const auto &i:v) {
        cur->next = new ListNode(i);
        cur = cur->next;
    }
    return ret->next;
}

int main() {
    ListNode *l1 = VectorToList({1,9});
    ListNode *l2 = VectorToList({1,0});
    Solution s;
    ListNode *result = s.addTwoNumbers(l1, l2);
    auto it = result;
    while (it) {
        cout << it->val << endl;
        it = it->next;
    }
}


