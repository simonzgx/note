#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <random>
#include <iostream>

using namespace std;

class RandomizedCollection {

    struct Helper {
        int index;
        Helper *next{};

        Helper(int idx) : index(idx), next(nullptr) {

        }
    };

public:
    /** Initialize your data structure here. */
    RandomizedCollection() {
        nums_.resize(10000);
    }

    /** Inserts a value to the collection. Returns true if the collection did not already contain the specified element. */
    bool insert(int val) {
        nums_[tail_] = val;
        auto *newItem = new Helper(tail_);
        tail_++;
        if (m_.count(val)) {
            newItem->next = m_[val];
            m_[val] = newItem;
            return false;
        } else {
            m_[val] = newItem;
            return true;
        }
    }

    /** Removes a value from the collection. Returns true if the collection contained the specified element. */
    bool remove(int val) {
        if (m_.count(val)) {
            auto next = m_[val];
            m_[nums_[tail_ - 1]]->index = next->index;
            nums_[next->index] = nums_[tail_ - 1];
            tail_--;
            if (next->next == nullptr) {
                delete next;
                m_.erase(val);
                return true;
            }
            m_[val] = next->next;
            delete next;
            return true;
        }
        return false;
    }

    /** Get a random element from the collection. */
    int getRandom() {
        return nums_[rand() % (tail_)];
    }

private:

    int tail_{};
    vector<int> nums_;
    default_random_engine e;
    unordered_map<int, Helper *> m_;
};

int main() {

    RandomizedCollection test;
    test.insert(0);
    test.insert(1);
    test.remove(0);
    test.insert(2);
    test.remove(1);
//    test.remove(2);
    for (int i = 0; i < 9; ++i)cout << test.getRandom() << endl;


    return 0;
}