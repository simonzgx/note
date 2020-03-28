//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    explicit ListNode(int x) : val(x), next(nullptr) {}
};

ListNode *deleteDuplicates(ListNode *head) {
    if(head== nullptr) return nullptr;
    if(head->next == nullptr) return head;
    ListNode *ret = new ListNode(-1), *cur = ret, *next= nullptr;
    ret->next = head;
    while (cur->next) {
        next = cur->next;
        while (next->next && next->next->val == next->val) {
            next = next->next;
        }
        if (next != cur->next) cur->next = next->next;
        else cur = cur->next;
    }
    return ret->next;
}

int main() {
    auto *head = new ListNode(1);
    ListNode *ret = head;
    int valueArr[] = {1};
    for (auto &i : valueArr) {
        head->next = new ListNode(i);
        head = head->next;
    }

    auto result = deleteDuplicates(ret);
    while (true) {
        if (result == nullptr)
            return 0;
        cout << result->val << endl;
        result = result->next;
    }
}


