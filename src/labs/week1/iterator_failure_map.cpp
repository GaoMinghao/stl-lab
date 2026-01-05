#include <iostream>
#include <map>

using namespace std;

int main() {
  map<int, string> m_c;
  for (int i = 4; i <= 10; ++i) {
    m_c.insert({i, to_string(i)});
  }
  auto it = m_c.find(5);
  cout << it->first << " " << it->second << endl;
  m_c.insert({3, to_string(11)});
  cout << it->first << " " << it->second << endl;
  for (auto it = m_c.begin(); it != m_c.end();) {
    if (it->first % 2 == 0) {
      it = m_c.erase(it);
    } else {
      ++it;
    }
  }
}
