#pragma once

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace stl_lab {

// 防止编译器优化掉无用的变量 (Anti-optimization)
template <typename T> inline void do_not_optimize(T &&value) {
  asm volatile("" : "+r,m"(value) : : "memory");
}

// 核心计时器
class NanosecondTimer {
public:
  template <typename Func> static double measure(Func func, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
               .count() /
           static_cast<double>(iterations);
  }
};

// 简单的结果打印工具
class BenchPrinter {
public:
  static void print_header(const std::string &title) {
    std::cout << "\n=== " << title << " ===\n";
    std::cout << std::left << std::setw(30) << "Operation" << std::setw(15)
              << "Time (ns)" << "\n";
    std::cout << std::string(45, '-') << "\n";
  }

  static void print_row(const std::string &name, double ns) {
    std::cout << std::left << std::setw(30) << name << std::fixed
              << std::setprecision(1) << ns << "\n";
  }
};

} // namespace stl_lab