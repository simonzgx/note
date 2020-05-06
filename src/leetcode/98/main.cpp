//
// Created by Simon on 2020/3/28.
//

#include <iostream>
#include <map>
#include <vector>
#include <stack>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:

    void dfs(TreeNode *node, int top, int bot, bool f1, bool f2) {
        if (!ans)return;
        if (f1 && node->val >= top)ans = false;
        if (f2 && node->val <= bot)ans = false;
        if (node->left) {
            dfs(node->left, node->val, bot, true, f2);
        }
        if (node->right) {
            dfs(node->right, top, node->val, f1, true);
        }
    }

    bool isValidBST(TreeNode *root) {
        if (!root)return true;
        if (root->left)dfs(root->left, root->val, 0, true, false);
        if (root->right)dfs(root->right, 0, root->val, false, true);
        return ans;
    }

private:
    bool ans{true};
};

int main() {
    Solution s;
    TreeNode n1{5}, n2{1}, n3{4}, n4{3}, n5{6};
    n1.left = &n2;
    n1.right = &n3;
    n3.left = &n4;
    n3.right = &n5;
    std::cout << s.isValidBST(&n1) << "\n";
}


