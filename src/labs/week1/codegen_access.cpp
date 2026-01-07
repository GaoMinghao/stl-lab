#include <vector>

// 选手 A: 裸奔访问
int access_unsafe(const std::vector<int> &v, size_t i) { return v[i]; }

// 选手 B: 安全访问
int access_safe(const std::vector<int> &v, size_t i) { return v.at(i); }