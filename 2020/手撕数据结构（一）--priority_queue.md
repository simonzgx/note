# 手撕数据结构---priority_queue

开始看C++ STL源码，涉及到一些标准库中的容器，这里做一下学习记录

* ### 描述

  **优先队列（priority_queue**）是STL标准库提供的一个**容器适配器（container adaptor）**，由于其底层数据结构默认是std::vector，没有真正实现一个容器，所以得以此命名。

  优先队列的本质是一个依赖于底层容器的**堆(heap)**，提供常数时间（`O(1)`）的最大元素查找，对数时间（`O(logn)`）的插入与释放。

* ### 模板形参

  ```c++
  template<class _Tp,
          class _Sequence = vector <_Tp>,
          class _Compare=std::less<typename _Sequence::value_type>>
  class priority_queue{}           
  ```

  其中：

  | 字段      | 含义                                                         |
  | --------- | ------------------------------------------------------------ |
  | _Tp       | 存储的元素类型。                                             |
  | _Sequence | 底层容器类型。容器必须满足序列容器 (SequenceContainer) 的要求，而其迭代器必须满足遗留随机访问迭代器 (LegacyRandomAccessIterator) 的要求。 |
  | _Compare  | 提供严格弱序的`Compare`类型。默认为`std::less`，**但是priority_queue会首先输出最大的元素，**因为源码实现的是`!cmp`则先出列。 |

* ### 成员

  * public member

  | 成员类型        | 定义                       |
  | --------------- | -------------------------- |
  | container_type  | Container                  |
  | value_compare   | Compare                    |
  | value_type      | Container::value_type      |
  | size_type       | Container::size_type       |
  | reference       | Container::reference       |
  | const_reference | Container::const_reference |

  * protected member

  | 成员类型     | 定义     |
  | ------------ | -------- |
  | Container c  | 底层容器 |
  | Compare comp | 比较函数 |

  * method

  | 方法名            | 说明                       |
  | ----------------- | -------------------------- |
  | priority_queue()  | 构造函数                   |
  | ~priority_queue() | 析构函数                   |
  | operator=         | assignment constructor     |
  | top               | 访问栈顶元素               |
  | empty             | 检查队列是否为空           |
  | size              | 返回容纳的元素数量         |
  | push              | 插入元素，并重新构建堆     |
  | emplace           | 原位构造元素，并重新构建堆 |
  | pop               | 删除栈顶元素               |

* ### 分析

  首先，由于成员变量的几个型别的定义依赖于`Container`，所以需要保证型别`_Tp`和`Container::value_type`一致。

  `priority_queue`主要提供三种构造方法:

  ```c++
  /* 默认构造 */
  priority_queue() : c() {}
  /* 提供自定义比较器的构造函数 */
  explicit priority_queue(const _Compare& __x) :  c(), comp(__x) {}
  /* 提供自定义底层容器的构造函数 */
  priority_queue(const _Compare& __x, const _Sequence& __s) 
      : c(__s), comp(__x) 
      { make_heap(c.begin(), c.end(), comp); }
  ```

  其中，第三种构造方法需要依据提供的容器构造一个堆。关于`make_heap`方法后面会深入探讨。

  `top`、`size`、`empty`三个方法并没有改变存储的数据，所以可以直接调用Container的相应方法，实现如下：

  ```c++
  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  const_reference top() const { return c.front(); }
  ```

  `push`、`emplace`、`pop`三个方法会修改`堆`，所以需要有对应的`堆`重构操作，实现如下：

  ```c++
  void emplace(_Tp &&args) {
      c.emplace_back(args);
      push_heap(c.begin(), c.end(), comp);
  }
  
  void push(const value_type &x) {
      try {
          c.push_back(x);
          push_heap(c.begin(), c.end(), comp);
      } catch (...) {
          c.clear();
      }
  }
  
  void pop() {
      try {
          pop_heap(c.begin(), c.end(), comp);
          c.pop_back();
      } catch (...) {
          c.clear();
      }
  }
  ```

  







