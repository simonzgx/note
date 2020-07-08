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

    explicit TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};


class Solution {
public:

    TreeNode *insertElement(TreeNode *root, int num) {

    }

    TreeNode *sortedArrayToBST(vector<int> &nums) {
        if (nums.empty())return nullptr;
        auto *root = new TreeNode(nums[0]);
        for (int i = 1; i < nums.size(); ++i) {
            root = insertElement(root, nums[i]);
        }

        return root;
    }
};


int main() {

}