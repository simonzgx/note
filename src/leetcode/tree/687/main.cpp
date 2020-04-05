//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>


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

    int longestUnivaluePath(TreeNode *root) {
        if (root == nullptr)return _ret;
        dfs(root, root->val);
        return _ret;
    }

    int dfs(TreeNode *node, int lastValue) {
        int leftLength = 0, rightLength = 0;
        if (node->left) {
            leftLength = dfs(node->left, node->val);
        }
        if (node->right) {
            rightLength = dfs(node->right, node->val);
        }
        if (leftLength + rightLength > _ret)_ret = leftLength + rightLength;
        if (node->val == lastValue)return max(leftLength, rightLength) + 1;
        return 0;
    }

private:
    int _ret{0};
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
* int param_1 = obj->get(key);
* obj->put(key,value);
*/

int main() {


}