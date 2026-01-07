add_executable(bench_week1_vector week1/vector_bench.cpp)

# 链接 Google Benchmark 库 (同时也链接了 pthread 等系统库)
target_link_libraries(bench_week1_vector PRIVATE benchmark::benchmark)