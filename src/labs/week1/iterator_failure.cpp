// 三类失效最小复现：vector 扩容、unordered rehash、map 插入/删除边界
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

using namespace std;

int main() {
  // vector 扩容
  vector<int> v_c(1, 1);
  vector<int>::iterator it = v_c.begin(); // 获取一个引用
  cout << *it << endl;
  v_c.push_back(2);    // 应当触发扩容
  cout << *it << endl; // 此处的 it 是未定义行为
  // unordered rehash

  unordered_map<int, string> u_m_c(1);
  u_m_c.insert({1, "a"});
  for (auto &it : u_m_c) {
    cout << it.first << " " << it.second << endl;
    u_m_c.insert({2, "b"});
    cout << it.first << " " << it.second << endl;
  }
}
