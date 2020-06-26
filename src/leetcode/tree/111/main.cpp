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
    int minDepth(TreeNode *root) {
        if (root == nullptr)return 0;
        queue<std::pair<TreeNode *, int>> path;
        TreeNode *cur = root;
        path.push({root, 1});
        while (!path.empty()) {
            auto item = path.front();
            path.pop();
            if (item.first->left) {
                path.push({item.first->left, item.second + 1});
            }
            if (item.first->right) {
                path.push({item.first->right, item.second + 1});
            }
            if (!item.first->left && !item.first->right) {
                return item.second;
            }
        }
        return 0;
    }
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
* int param_1 = obj->get(key);
* obj->put(key,value);
*/

int main() {
    TreeNode n1(3), n2(9), n3(20), n4(15), n5(7);
    n1.right = &n3;
    n1.left = &n2;
    n3.right = &n4;
    n3.left = &n5;
    Solution s;
    cout << s.minDepth(&n1);
}