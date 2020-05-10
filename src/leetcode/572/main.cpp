#include <vector>


using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode() : val(0), left(nullptr), right(nullptr) {}

    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}

    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};


template<typename T>
void func(T &t) {

}

struct Widget {
    Widget() = default;
};


Widget factory() {
    return Widget();
}

int main() {
    func(factory());
    return 0;
}