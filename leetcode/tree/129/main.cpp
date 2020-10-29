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

    void dfs(TreeNode *node, vector<int> &trace) {
        trace.push_back(trace.back() * 10 + node->val);
        if (node->left == nullptr && node->right == nullptr) {
            ans += trace.back();
            trace.pop_back();
            return;
        }
        if (node->left) {
            dfs(node->left, trace);
        }
        if (node->right) {
            dfs(node->right, trace);
        }
        trace.pop_back();
    }


    int sumNumbers(TreeNode *root) {
        if (root == nullptr)return 0;
        vector<int> trace;
        trace.push_back(0);
        dfs(root, trace);
        return ans;
    }

private:
    int ans{};
};

int main() {
    Solution s;
    TreeNode n1(4), n2(9), n3(0), n4(4), n5(5), n6(1);
    n1.left = &n2;
    n1.right = &n3;
    n2.left = &n5;
    n2.right = &n6;
    cout << s.sumNumbers(nullptr) << endl;
}