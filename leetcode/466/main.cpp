//
// Created by Simon on 2020/4/4.
//
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>


using namespace std;

struct Node {
    int key;
    int value;
    int times;
    int lastUse;

    friend bool operator<(const Node &left, const Node &right) {
        if (left.times == right.times)return left.lastUse < right.lastUse;
        return left.times < right.times;
    }

    friend ostream &operator<<(ostream &stream, const Node &n) {
        stream << "key:" << n.key << " value:" << n.value << " times:" << n.times << " lastUse" << n.lastUse << endl;
        return stream;
    }
};


class LFUCache {
public:
    explicit LFUCache(int capacity) : _cap(capacity) {

    }

    int get(int key) {
        auto it = _kvMap.find(key);
        if(it == _kvMap.end())return -1;
        Node cache = it->second;
        _kvMap[key].lastUse = time++;
        _kvMap[key].times ++;
        _ktMap.erase(cache);
        cache.times++;
        cache.lastUse = time;
        _ktMap.insert(cache);
        return it->second.value;
    }

    void put(int key, int value) {
        if (_cap == 0)return;
        auto it = _kvMap.find(key);
        if (it == _kvMap.end()) {
            if (_kvMap.size() == _cap) {
                auto begin = _ktMap.begin();
                _kvMap.erase(begin->key);
                _ktMap.erase(begin);
            }
            Node n{key, value, 0, time++};
            _kvMap[key] = n;
            _ktMap.emplace(n);
        } else {
            Node cache = it->second;
            _ktMap.erase(cache);
            cache.lastUse = time++;
            cache.times++;
            cache.value = value;
            _ktMap.emplace(cache);
            _kvMap[key]= cache;
        }
    }

private:
    size_t _cap;
    int time{0};
    unordered_map<int, Node> _kvMap;
    set<Node> _ktMap;
};

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
* int param_1 = obj->get(key);
* obj->put(key,value);
*/

int main() {
    LFUCache cache(2);
    cache.put(2, 1);
    cache.put(2, 2);
    cout<<cache.get(2)<<endl;       // 返回 1
    cache.put(1,1);
    cache.put(4,1);
    cout<<cache.get(2)<<endl;

}