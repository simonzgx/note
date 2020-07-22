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

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    explicit TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:

    vector<int> rightSideView(TreeNode *root) {
        vector<int> ans;
        if (!root)return ans;
        vector<TreeNode *> s1, s2;
        unsigned depth = 0;
        s1.emplace_back(root);
        while (true) {
            depth++;
            if (depth & (unsigned) 1) {
                ans.emplace_back(s1[s1.size() - 1]->val);
                for (const auto &node:s1) {
                    if (node->left)s2.emplace_back(node->left);
                    if (node->right)s2.emplace_back(node->right);
                }
                if (s2.empty())break;
                s1.clear();
                continue;
            }
            ans.emplace_back(s2[s2.size() - 1]->val);
            for (const auto &node:s2) {
                if (node->left)s1.emplace_back(node->left);
                if (node->right)s1.emplace_back(node->right);
            }
            if (s1.empty())break;
            s2.clear();
        }

        return ans;
    }

};

int main() {
    Solution s;
    TreeNode n1(1), n2(2), n3(3), n4(5), n5(4);
    n1.left = &n2;
    n2.right = &n4;
    n1.right = &n3;
    n3.right = &n5;
    auto ans = s.rightSideView(&n1);
    for (int &v: ans) {
        cout << v << " ";
    }
    cout << endl;
}