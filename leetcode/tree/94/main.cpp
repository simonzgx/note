//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <mmcobj.h>
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

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};


class Solution {
public:
    Solution() = default;

    void dfs(TreeNode *node) {
        if (node->left) {
            dfs(node->left);
        }
        ret.push_back(node->val);
        if (node->right) {
            dfs(node->right);
        }
    }

    vector<int> inorderTraversal(TreeNode *root) {
        if (root == nullptr)return ret;
        dfs(root);
        return ret;
    }


private:
    vector<int> ret;
    vector<TreeNode *> path;
    set<TreeNode *> _m;
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
* int param_1 = obj->get(key);
* obj->put(key,value);
*/

int main() {
    TreeNode n1(1), n2(2), n3(3);
    n1.right = &n2;
    n2.left = &n3;
    Solution s;
    s.inorderTraversal(&n1);
}