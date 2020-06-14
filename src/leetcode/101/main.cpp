#include <iostream>
#include <stack>


struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:

    void dfs(TreeNode *l, TreeNode *r) {
        if(l->val != r->val){
            _ans = false;
            return;
        }
        if(l->left != nullptr && r->left != nullptr){
            dfs(l->left, r->left);
        }else if(l->left == nullptr && r->left == nullptr){
            if(l->right != nullptr && )
        }else{
            _ans = false;
            return;
        }
    }


    bool isSymmetric(TreeNode *root) {
        _ans = true;
        if (nullptr == root)return _ans;
        if (root->left != nullptr && root->right != nullptr) {
            dfs(root->left, root->right);
        } else return root->left == nullptr && root->right == nullptr;

        return _ans;
    }

private:
    bool _ans;
};