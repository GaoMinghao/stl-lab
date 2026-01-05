// 三类失效最小复现：vector 扩容、unordered rehash、map 插入/删除边界
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// 1. vector 插入对迭代器的影响（内存未重新分配）
void vector_insert_without_realloc() {
  vector<int> v1;
  v1.reserve(20);
  cout << "size of v1: " << v1.size() << endl;
  vector<int>::iterator it2, it3, it6;
  for (int i = 0; i < 10; i++) {
    v1.push_back(i);
    if (i == 2)
      it2 = v1.begin() + i; // 重新获取有效的迭代器
    if (i == 3)
      it3 = v1.begin() + i;
    if (i == 6)
      it6 = v1.begin() + i;
  }
  cout << "value of it2: " << *it2 << endl;
  cout << "value of it3: " << *it3 << endl;
  cout << "value of it6: " << *it6 << endl;
  v1.insert(it3, 100);
  cout << "value of it2 after insert: " << *it2
       << endl; // 插入点之前的迭代器没有失效
  cout << "value of it3 after insert: " << *it3
       << endl; // 插入点指向了最新的数据（被插入的数据）
  cout << "value of it6 after insert: " << *it6
       << endl; // 插入点之后的数据，在没有重新分配内存的情况下，会失效
}

// 2. vector 插入对迭代器的影响（产生内存重新分配）
void vector_insert_with_realloc() {
  vector<int> v2{1, 2, 3, 4, 5, 6, 7, 8};
  cout << "size of v2: " << v2.size() << endl;
  vector<int>::iterator it_2_1 = v2.begin() + 1;
  cout << "value at index 1: " << *it_2_1 << endl;
  for (int i = 9; i < 20; ++i) {
    v2.push_back(i);
  }
  for_each(v2.cbegin(), v2.cend(), [](const auto &val) { cout << val << ","; });
  cout << endl;
  // 此时应该发生了内存重新分配， it_2_1 应该失效了，此处会产生 core
  v2.insert(it_2_1, 1);
}

// 3. vector
// 删除对迭代器的影响（删除点之前的迭代器仍然有效，删除点之后的迭代，引用，指针都会失效）
void vector_erase() {
  vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  vector<int>::iterator it0 = v.begin();
  cout << "iterator at 0 before erase: " << *it0 << endl;
  vector<int>::iterator it3 = v.begin() + 3;
  cout << "iterator at 3 before erase: " << *it3 << endl;
  vector<int>::iterator it7 = v.begin() + 7;
  cout << "iterator at 7 before erase: " << *it7 << endl;
  auto it_end = v.end();
  v.erase(it3);
  cout << "iterator at 0 after erase: " << *it0 << endl;
  cout << "iterator at 3 after erase: " << *it3 << endl;
  cout << "iterator at 7 after erase: " << *it7 << endl;
  // 此处的 it_end 已失效
  for (auto it = v.begin(); it != it_end; ++it) {
    cout << *it << ", ";
  }
  cout << endl;
  // erase 接口会返回被删除位置的下一个迭代器，所以循环中使用 erase 需要注意
  for (auto it = v.begin();
       it != v.end();) { // 此处不能用++it，否则it会被加两次，部分元素就被跳掉了
    if (*it % 2 == 0) {
      it = v.erase(it); // erase返回的是it的下一位
    } else {
      ++it;
    }
  }
  // prefer 更现代化的写法
}

vector<int> get_vector() { return vector<int>{0, 1, 3, 4, 5}; }

// 4. for-range的原理
void explain_for_range() {
  for (auto val : get_vector()) {
    cout << val << endl;
  }
  // 等价于
  auto &&__range = get_vector();
  vector<int>::iterator __begin = __range.begin();
  vector<int>::iterator __end = __range.end();
  for (; __begin != __end; ++__begin) {
    int val = *__begin;
    cout << val << endl;
  }
}

int main() {
  vector_insert_without_realloc();
  // vector_insert_with_realloc();
  vector_erase();
}
