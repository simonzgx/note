//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <map>

using namespace std;

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}

    explicit ListNode(int x) : val(x), next(nullptr) {}

    ListNode(int x, ListNode *next) : val(x), next(next) {}
};


class Solution {
public:
    ListNode *swapPairs(ListNode *head) {
        if (!head || !head->next) {
            return head;
        }
        ListNode newHead;
        newHead.next = head;
        ListNode *pre = &newHead, *fst = head;
        while (fst && fst->next) {
            pre->next = fst->next;
            fst->next = fst->next->next;
            pre->next->next = fst;
            pre = fst;
            fst = pre->next;
        }
        return newHead.next;
    }
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
* int param_1 = obj->get(key);
* obj->put(key,value);
*/

int main() {
    ListNode a(1), b(2), c(3), d(4),e(5);
    a.next = &b;
    b.next = &c;
    c.next = &d;
    d.next = &e;
    Solution s;
    auto newHead = s.swapPairs(&a);
    while (newHead) {
        cout << newHead->val << endl;
        newHead = newHead->next;
    }

}