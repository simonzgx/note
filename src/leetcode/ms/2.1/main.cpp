//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <unordered_map>

using namespace std;


struct ListNode {
    int val;
    ListNode *next;

    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
    ListNode *removeDuplicateNodes(ListNode *head) {
        if (head == nullptr) {
            return nullptr;
        }
        unordered_map<int, int> v;
        v[head->val]++;
        ListNode *cur;
        cur = head;
        while (cur->next) {
            if (v[cur->next->val]) {
                cur->next = cur->next->next;
            } else {
                v[cur->next->val]++;
                cur = cur->next;
            }
        }
        return head;
    }
};

int main() {

}


