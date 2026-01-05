#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int test_unordered_map_insert() {
  // unordered rehash
  unordered_map<int, string> u_m_c;
  u_m_c.reserve(3);

  u_m_c.insert({1, "a"});
  u_m_c.insert({2, "b"});
  size_t before_size = u_m_c.bucket_count();
  cout << "Bucket count after fist insert: " << before_size << endl;
  auto it1 = u_m_c.find(1);
  auto it2 = u_m_c.find(2);
  u_m_c.insert({3, "c"});
  size_t after_size = u_m_c.bucket_count();
  if (before_size != after_size) {
    cout << "unordered map rehased." << endl;
    return 1;
  }
  cout << "============= test iterator validation before rehash. ============="
       << endl;
  cout << it1->first << ": " << it1->second << endl;
  cout << it2->first << ": " << it2->second << endl;
  cout << "============= test iterator validation after rehash. ============="
       << endl;
  for (auto &it : u_m_c) {
    cout << it.first << " " << it.second << endl;
    // 插入，触发了rehash，此时迭代器失效，无法再继续处理，但是当前指向的元素还是有效的
    u_m_c.insert({4, "d"});
    cout << "Bucket count after insert in loop:  " << u_m_c.bucket_count()
         << endl;
    cout << it.first << " " << it.second << endl;
  }
  return 0;
}

void test_unordered_map_erase() {
  unordered_map<int, string> umc;
  for (int i = 0; i < 10; ++i) {
    umc.insert({i, to_string(i)});
  }
  auto it1 = umc.find(1);
  auto it5 = umc.find(5);
  auto it8 = umc.find(8);
  umc.erase(it5);
  cout << "直接删除，删除点前后的迭代器不会失效" << endl;
  cout << it1->first << " " << it1->second << endl;
  cout << it8->first << " " << it8->second << endl;
  // 循环删除，需要注意 erase 返回的是下一个迭代器
  for (auto it = umc.begin(); it != umc.end();) {
    if (it->first % 2 == 0) {
      it = umc.erase(it); // 删除最后一个元素之后会返回 end()
      if (it != umc.end()) {
        cout << it->first << " " << it->second << endl;
      }
    } else {
      it++;
    }
  }
// CPP20后应该使用更现代化的写法：
#if __cplusplus >= 202002L
  std::erase_if(umc, [](const auto &item) { return item.first % 2 == 0; });
#endif
}

int main() {
  test_unordered_map_insert();
  test_unordered_map_erase();
}
