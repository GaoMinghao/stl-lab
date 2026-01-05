#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>

struct Metrics {
  static std::int64_t c_ctor, m_ctor, c_asgn, m_asgn, dtor;

  static void reset() { c_ctor = m_ctor = c_asgn = m_asgn = dtor = 0; }

  static void print(const char *tag) {
    std::printf(
        "[%s] C_Ctor=%lld M_Ctor=%lld C_Asgn=%lld M_Asgn=%lld Dtor=%lld\n", tag,
        (long long)c_ctor, (long long)m_ctor, (long long)c_asgn,
        (long long)m_asgn, (long long)dtor);
  }
};

// TODO: 在 cpp 文件里定义静态成员（这里单文件就直接在下方定义）
int64_t Metrics::c_ctor = 0;
int64_t Metrics::m_ctor = 0;
int64_t Metrics::c_asgn = 0;
int64_t Metrics::m_asgn = 0;
int64_t Metrics::dtor = 0;

struct Tracked {
  int x{0};

  Tracked() = default;
  explicit Tracked(int v) : x(v) {}

  // TODO: 拷贝构造：计数 + 拷贝 x
  Tracked(const Tracked &other) : x(other.x) { ++Metrics::c_ctor; }

  // TODO: 移动构造（noexcept）：计数 + 移动/转移 x（你可以把 rhs.x 改成 -1
  // 方便观察）
  Tracked(Tracked &&other) noexcept : x(other.x) {
    ++Metrics::m_ctor;
    other.x = -1;
  }

  // TODO: 拷贝赋值：计数 + 赋值 x
  Tracked &operator=(const Tracked &other) {
    ++Metrics::c_asgn;
    x = other.x;
    return *this;
  }

  // TODO: 移动赋值（noexcept）：计数 + 赋值/转移 x
  Tracked &operator=(Tracked &&other) noexcept {
    ++Metrics::m_asgn;
    x = other.x;
    other.x = -1;
    return *this;
  }

  // TODO: 析构：计数
  ~Tracked() { ++Metrics::dtor; }
};

static void print_vec(const char *name, const std::vector<Tracked> &v) {
  std::printf("  %s: size=%zu cap=%zu data=%p\n", name, v.size(), v.capacity(),
              (const void *)v.data());
}

static std::vector<Tracked> make_vec(std::size_t n) {
  std::vector<Tracked> v;
  v.reserve(n);
  for (std::size_t i = 0; i < n; ++i)
    v.emplace_back((int)i);
  return v;
}

static void demo_copy_assign(std::size_t n) {
  auto src = make_vec(n);
  auto dst = make_vec(n);

  std::puts("\n== copy assignment: dst = src ==");
  print_vec("src(before)", src);
  print_vec("dst(before)", dst);

  Metrics::reset();
  dst = src;

  print_vec("src(after )", src);
  print_vec("dst(after )", dst);
  Metrics::print("copy_assign");
}

static void demo_move_assign(std::size_t n) {
  auto src = make_vec(n);
  auto dst = make_vec(n / 2);

  std::puts("\n== move assignment: dst = std::move(src) ==");
  auto src_ptr = src.data();
  print_vec("src(before)", src);
  print_vec("dst(before)", dst);

  Metrics::reset();
  dst = std::move(src);

  print_vec("src(after )", src);
  print_vec("dst(after )", dst);
  Metrics::print("move_assign");

  std::printf("  note: src.data(before)=%p, dst.data(after)=%p\n",
              (void *)src_ptr, (void *)dst.data());
}

static void demo_swap(std::size_t a_n, std::size_t b_n) {
  auto a = make_vec(a_n);
  auto b = make_vec(b_n);

  std::puts("\n== swap: a.swap(b) ==");
  auto a_ptr = a.data();
  auto b_ptr = b.data();
  print_vec("a(before)", a);
  print_vec("b(before)", b);

  Metrics::reset();
  a.swap(b);

  print_vec("a(after )", a);
  print_vec("b(after )", b);
  Metrics::print("swap");

  std::printf("  note: a.data(before)=%p -> a.data(after)=%p\n", (void *)a_ptr,
              (void *)a.data());
  std::printf("        b.data(before)=%p -> b.data(after)=%p\n", (void *)b_ptr,
              (void *)b.data());
}

int main() {
  demo_copy_assign(8);
  demo_move_assign(16);
  demo_swap(16, 8);
}
