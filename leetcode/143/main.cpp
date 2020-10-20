#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}

    ListNode(int x) : val(x), next(nullptr) {}

    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

template<typename Node>
Node *createNode(vector<Node> &nodes) {
    if (nodes.empty())return nullptr;
    for (int i = 0; i < nodes.size() - 1; ++i) {
        nodes[i].next = &nodes[i + 1];
    }
    nodes.back().next = nullptr;
    return &*nodes.begin();
}

template<typename Node>
Node *createNode(Node n1, Node n2, ...) {

}


class Solution {
public:
    void reorderList(ListNode *head) {
        if (head == nullptr)return;
        vector<ListNode *> nodes;
        ListNode *cur = head;
        while (cur) {
            nodes.push_back(cur);
            cur = cur->next;
        }
        int i = 0, j = nodes.size() - 1;
        while (i < j) {
            (*nodes[i]).next = nodes[j];
            (*nodes[j]).next = nodes[i + 1];
            i++;
            j--;
        }
        (*nodes[i]).next = nullptr;
    }
};

int main() {
    vector<ListNode> nodes{1, 2, 3, 4};
    auto *root = createNode<ListNode>(nodes);
    Solution s;
    s.reorderList(root);
    while (root) {
        cout << root->val << endl;
        root = root->next;
    }
}