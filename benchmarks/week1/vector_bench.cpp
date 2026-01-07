#include <benchmark/benchmark.h>
#include <numeric>
#include <vector>

using namespace std;

// 1. Copy Benchmark
static void BM_VectorCopy(benchmark::State &state) {
  // 获取传入的参数 N
  size_t N = state.range(0);

  // 构造测试数据 (不在计时范围内)
  vector<int> source(N, 42);

  // 核心循环
  for (auto _ : state) {
    // 测试目标：拷贝构造
    vector<int> copy = source;
    // 防止优化：确保 copy 被“使用”
    benchmark::DoNotOptimize(copy);
  }

  // 设置处理的字节数，方便报告显示吞吐量 (Bytes/s)
  state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(N) *
                          sizeof(int));
}

// 2. Move Benchmark
static void BM_VectorMove(benchmark::State &state) {
  size_t N = state.range(0);

  // 预先准备足够多的数据，避免在计时循环里做构造
  // Google Bench 默认跑的时间不长，但在 Quant 场景我们可能想多测点
  // 这里采用“每次循环重新构造”的策略如果 N 很大可能会导致内存压力
  // 但对于 Move 来说，我们可以用一个小技巧：Batch Creation

  for (auto _ : state) {
    // 暂停计时：构造源对象
    state.PauseTiming();
    vector<int> source(N, 42);
    state.ResumeTiming();

    // 恢复计时：执行 Move
    vector<int> dest = std::move(source);
    benchmark::DoNotOptimize(dest);
  }
}

// 3. Swap Benchmark
static void BM_VectorSwap(benchmark::State &state) {
  size_t N = state.range(0);
  vector<int> v1(N, 1);
  vector<int> v2(N, 2);

  for (auto _ : state) {
    v1.swap(v2);
    // Swap 代价极低，DoNotOptimize 可能会占一定比例，但这是必要的
    benchmark::DoNotOptimize(v1);
  }
}

// 注册测试用例，并定义参数范围 (N)
// Range(8, 8<<10) 意味着 N 取 8, 64, 512, 4096... 直到 8192
// 我们可以自定义更符合 Quant 场景的大小，比如 100, 10k, 1M

// Copy 随 N 线性增长
BENCHMARK(BM_VectorCopy)->Arg(100)->Arg(10000)->Arg(1000000);

// Move 和 Swap 是 O(1)，理论上对 N 不敏感，但我们可以验证这一点
BENCHMARK(BM_VectorMove)->Arg(100)->Arg(10000)->Arg(1000000);
BENCHMARK(BM_VectorSwap)->Arg(100)->Arg(10000)->Arg(1000000);

// 自动生成 main 函数
BENCHMARK_MAIN();