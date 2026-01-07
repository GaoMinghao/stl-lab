## Labs 语义线
1. vector本质上是一段连续的内存，因此，凡是发生连续内存变动的场景（在末尾插入时如果内存不够会重新申请大块内存并将原来内存中的对象 move 过去；在中间插入时，如果不发生扩容，则插入点之后的所有对象都要向后移动；erase时，删除点之后的所有对象都要向前移动）都需要注意，一旦内存发生变化，那么迭代器，指针，引用都会失效
2. unordered_map 内部分为 bucket 数组和 node 两个部分，如果bucket使用程度过高则会发生 rehash，此时迭代器都会失效，但是通过解引用原先迭代器获得的元素的引用并不会失效，因为这部分元素实际上并不存储在 bucket 中，这一点与 vector 是有区别的；erase 不会 bucket rehash，因此不会失效
3. map 底层是红黑树，因此 insert 和 erase 都会导致树重排，但是重排只是 prev 和 next 指针指向发生了变化，迭代器不会失效
4. erase 接口会返回下一个迭代器的地址，在循环中使用 erase 需要注意
5. 尽量使用比较优雅的写法来处理循环 erase
```cpp
#if __cplusplus >= 202002L
std::erase_if(c.begin(), c.end(), [](auto &val)( return false; /*pred*/));
#else
c.erase(std::remove_if(c.begin(), c.end(),pred),c.end());
#endif
```
## 性能线
1. 使用 Google benchmark 进行测试
2. copy的性能最差，move和swap只需要交换三个指针，性能最佳且恒定，但是在 Google bench 中析构的开销会被计算在内（实际开发中析构确实也是需要花时间的）

## Codegen
1. 观察到 at() 会进行安全检查，判断是否越界，性能较差