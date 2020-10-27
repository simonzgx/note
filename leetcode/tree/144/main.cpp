//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <stack>

using namespace std;

// Definition for a Node.
class Node {
public:
    int val;
    Node *left;
    Node *right;
    Node *next;

    Node() : val(0), left(nullptr), right(nullptr), next(nullptr) {}

    explicit Node(int _val) : val(_val), left(nullptr), right(nullptr), next(nullptr) {}

    Node(int _val, Node *_left, Node *_right, Node *_next)
            : val(_val), left(_left), right(_right), next(_next) {}
};

class Solution {
public:

    void bfs(Node *node) {

    }

    Node *connect(Node *root) {
        vector<Node *> stack1, stack2;
        vector<Node *> *s1 = &stack1, *s2 = &stack2;
        s1->push_back(root);
        s1->push_back(nullptr);
        int i = 0, j;
        while (s1->front()) {
            for (j = 0; j < s1->size() - 1; ++j) {
                (*s1)[j]->next = (*s1)[j + 1];
                if ((*s1)[j]->left) {
                    s2->push_back((*s1)[j]->left);
                    s2->push_back((*s1)[j]->right);
                }
            }
            s2->push_back(nullptr);
            s1->clear();
            if (i % 2 == 0) {
                s1 = &stack2;
                s2 = &stack1;
            } else {
                s1 = &stack1;
                s2 = &stack2;
            }
            ++i;
        }
        return root;
    }
};

int main() {
    Solution s;
    Node n1(1), n2(2), n3(3), n4(4), n5(5), n6(6), n7(7);
    n1.left = &n2;
    n1.right = &n3;
    n2.left = &n4;
    n2.right = &n5;
    n3.left = &n6;
    n3.right = &n7;
    auto ans = s.connect(&n1);
    cout << ans->left->next->val << endl;
}