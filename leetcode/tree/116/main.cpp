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
class TreeNode {
public:
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode() : val(0), left(nullptr), right(nullptr) {}

    explicit TreeNode(int _val) : val(_val), left(nullptr), right(nullptr) {}

};

class Solution {
public:


    vector<int> preorderTraversal(TreeNode *root) {
        vector<int> ans;
        if (root == nullptr)return ans;
        stack<TreeNode *> s;
        s.push(root);
        while (!s.empty()) {
            auto node = s.top();
            s.pop();
            ans.emplace_back(node->val);
            if (node->right) {
                s.push(node->right);
            }
            if (node->left) {
                s.push(node->left);
            }
        }
        return ans;
    }
};

int main() {
    Solution s;

}