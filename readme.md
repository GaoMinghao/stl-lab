# Plan v1.0.1：Quant-Dev 增强版 STL 学习与训练计划（15h/周，x86-64，GCC 13）

> 核心教材：
>
> - 《C++ 标准库（第2版）》（C++SL，中英文版均可互证）
> - 《STL 源码剖析》（STL-SCA，取思想/结构/技巧，不追逐逐行一致）

---

## 目录

- [0. 总目标与最终产出](#0-总目标与最终产出)
- [1. 核心原则（固定不变）](#1-核心原则固定不变)
- [2. 周节奏模板（15h/周）](#2-周节奏模板15h周)
- [3. 工具链与测量纪律（x86-64 / GCC 13）](#3-工具链与测量纪律x86-64--gcc-13)
- [4. 10 周主线（按语义线 + 性能线分轨）](#4-10-周主线按语义线--性能线分轨)
- [5. Capstone（持续）](#5-capstone持续)
- [6. “过时 vs 经典 vs 现代对应物”对照表（每周必做）](#6-过时-vs-经典-vs-现代对应物对照表每周必做)
- [7. 计划确认点（当前阶段）](#7-计划确认点当前阶段)

---

## 0. 总目标与最终产出

### 目标

在 10 周内建立“量化开发可用”的 STL 能力：

- **选型能力**：基于证据选择容器/算法（延迟敏感 + tail latency 导向），而非凭经验。
- **语义把握**：复杂度、迭代器/引用失效、异常安全、allocator 传播规则等关键契约可准确复述并能写成断言。
- **底层视角**：用 GCC 13 的向量化报告与反汇编验证关键路径（热/冷路径、是否向量化、missed 原因）。
- **工程沉淀**：形成可复用“规则库”（if-then + 例外边界），可迁移到订单簿/行情缓存/事件队列等组件。

### 最终产出（仓库内）

- `docs/weekX.md` × 10（每周一份：结论表 + 证据链 + 规则库更新）
- `tests/`：关键语义回归测试（每周 ≥ 2）
- `benchmarks/`：关键基准（每周 ≥ 1）
- `tools/`：固定的 codegen 与测量脚本
- `capstone/`：量化语境小项目（含“证据驱动选型报告”）

---

## 1. 核心原则（固定不变）

1. **Allocator 前置**：Week 2（vector）与 Week 6（unordered）早绑定 allocator，训练尾延迟/抖动。
2. **LinearAllocator 仅安全姿势**：主线严格限定 `reserve(N) + 断言不扩容`，避免 bump allocator 噪声污染结论。
3. **Bench + Codegen 双轨**：每周至少 1 个基准 + 1 次 GCC13 向量化/反汇编检查。
4. **《STL 源码剖析》只取思想**：固定用“过时 vs 经典 vs 现代对应物”三列表，不追逐 SGI STL 逐行复刻。
5. **工程可迁移**：FetchContent 管依赖；Sanitizer 仅 Debug；`-march=native` 用开关控制。
6. **标准策略**：主线实验用 C++14；C++20 仅用于可选工具，不污染主线实验。
7. **观测工具硬门槛**：Tracked（生命周期/赋值计数）+ CountingAllocator（分配行为计数）。
8. **`operator[]` 无边界检查**：检查关注点在 `at()` / debug 宏 / sanitizer 插桩。
9. **语义线与性能线分轨（关键补丁）**：
   - **语义线**允许 `Tracked` 等副作用观测；
   - **性能/向量化线**必须用 **trivial 类型（int 或 POD struct）**，否则 missed 可能只是副作用抑制优化，缺乏诊断价值。
10. **x86 口径加严**：`vzeroupper` 可能是编译器为规避 AVX/YMM 与 legacy SSE 混用切换开销的防御性插入，但**不能单独作为性能结论依据**。

---

## 2. 周节奏模板（15h/周）

### 时间分配（建议固定）

- Reading：5h（C++SL 3h + STL-SCA 2h）
- Labs：4h（2 个必做语义实验，可断言）
- Bench + Codegen：4h（Bench 1–2 个 + Codegen 1 次）
- 规则库沉淀：2h（本周最重要 3 条规则 if-then + 例外边界）

### 每周通过标准（Done Criteria）

- [ ] `docs/weekX.md` 1 份，包含：
  - [ ] 结论表 ≥ 6 条（带适用边界）
  - [ ] 失效/异常安全摘要 ≥ 5 条
  - [ ] “过时 vs 经典 vs 现代对应物” ≥ 3 行
- [ ] 单测 ≥ 2 个（关键契约写成断言）
- [ ] Labs ≥ 2 个（输入/输出/预期现象明确）
- [ ] Bench ≥ 1 个（写清 N、分布、编译配置；输出均值 + 尾部近似指标）
- [ ] Codegen ≥ 1 次（保存 vec 报告关键行 + 汇编关键片段）
- [ ] 规则库更新：3 条 if-then + 例外边界（可迁移到量化系统）

---

## 3. 工具链与测量纪律（x86-64 / GCC 13）

### 构建配置（建议）

- 默认：`RelWithDebInfo`（等价思路：`-O3 -g -fno-omit-frame-pointer`）
- Debug 才开：`ASan + UBSan`
- `-march=native`：默认关闭；仅作为“本机极限”对照可选开启

### Bench 尾延迟/抖动（最低要求）

- 同一 bench 运行多次（或 `--benchmark_repetitions`），至少记录：
  - 均值/中位数（若可得）
  - 尾部近似指标：重复多次的最大值或高分位近似
- 报告里注明环境：CPU 频率策略、后台负载、是否绑定核（不必极致，但必须可追溯）

### GCC 13 Codegen（最低要求）

- 向量化报告：`-fopt-info-vec-all=vec.log`
  - 结论必须落为：**vectorized** 或 **missed + 原因**
- 汇编/反汇编：只抓关键符号附近片段（不贴长汇编）

---

## 4. 10 周主线（按语义线 + 性能线分轨）

> 注：阅读以主题为导向；具体页码/章节号可在每周 `docs/weekX.md` 记录（中英文版互证）。

---

### Week 1：STL 契约层总览（值语义、复杂度、异常安全、失效规则框架）

**Reading**

- C++SL：容器通用接口；赋值/移动/交换复杂度与语义；异常安全要点
- STL-SCA：STL 六大部件总览；泛型编程结构（iterator/algorithm/container/functor）

**Labs（语义线）**

- [ ] 三类失效最小复现：vector 扩容、unordered rehash、map 插入/删除边界
- [ ] swap vs copy assignment vs move assignment 的语义差异（用观测工具固化直觉）

**Bench（性能线：trivial 类型）**

- [ ] `vector<int>` 上 copy/move/swap 对比（建立基线）

**Codegen**

- [ ] `operator[]` vs `at()`：分支/异常路径差异（观察）

**交付**

- [ ] `docs/week1.md`：失效规则表（初版）+ 复杂度保证摘要（初版）

---

### Week 2：Vector & Memory（Allocator early-binding）

**Reading**

- C++SL：vector（reserve/resize、扩容、插入删除、失效规则、复杂度）
- STL-SCA：vector 三指针模型；uninitialized\_\* 思想（构造/搬迁/销毁分离）

**Labs（语义线：必做）**

- [ ] Tracked 三路径赋值形态（闭环）：
  - `dst.size()==src.size()` → 以 copy-assign 为主
  - `dst.size()>src.size()` → 尾部析构出现
  - `dst.capacity()<src.size()` → reallocate（move/copy ctor + dtor）
- [ ] CountingAllocator 观测扩容：NoReserve vs Reserve（alloc 次数、峰值 bytes）
- [ ] LinearAllocator 安全姿势：`reserve(N)+断言 capacity 不变`

**Bench（性能线：必做，trivial + heavy 分开）**

- [ ] Growth：NoReserve vs Reserve（trivial 类型）
- [ ] Heavy：`vector<string(1KB)>` copy vs move vs swap（尾延迟 + 内存峰值）

**Codegen（性能线：必做，必须用 trivial 类型）**

- [ ] `std::accumulate` vs 手写 loop：vec 报告（vectorized 或 missed 原因）
- [ ] push_back 热/冷路径识别（容量检查 + 冷路径）

**交付**

- [ ] `docs/week2.md`：reserve 规则、assign/析构/扩容三路径、Linear 安全边界

---

### Week 3：Deque（队列负载、分段存储、抖动）

**Reading**

- C++SL：deque（随机访问、两端操作、失效规则）
- STL-SCA：deque 分段结构与迭代器复杂性（思想）

**Labs（语义线）**

- [ ] deque 失效规则复现（插入/两端操作）
- [ ] 简化事件队列：push_back + pop_front

**Bench（性能线）**

- [ ] `deque` vs `vector + head_index`（环形缓冲思路）吞吐/抖动对比

**Codegen**

- [ ] `deque[i]` vs `vector[i]` 地址计算差异（定性）

---

### Week 4：List / forward_list（稳定迭代器、splice、性能陷阱）

**Reading**

- C++SL：list/forward_list（splice/merge/sort 语义与复杂度）
- STL-SCA：链表节点与 splice 的经典实现

**Labs（语义线）**

- [ ] splice O(1) 搬运实验（搬运而非拷贝）
- [ ] forward_list 单向约束：可用/不可用算法边界

**Bench（性能线）**

- [ ] 遍历与中间插删：list 可能输给 vector 的原因（cache miss）

**可选增强（有 perf 再做，不阻塞）**

- [ ] perf：IPC / cache miss 对比（附录记录）

---

### Week 5：map/set（有序结构与订单簿语义入口）

**Reading**

- C++SL：map/set（比较器契约、lower_bound、区间遍历）
- STL-SCA：RB-tree 不变式与旋转/平衡思想（理解即可）

**Labs（语义线）**

- [ ] “坏比较器”反例（严格弱序破坏的后果）
- [ ] L2 档位结构：`map<price, qty>` 区间查询与快照

**Bench（性能线）**

- [ ] `map` vs `sorted vector + binary_search`：更新频率不同下对比（证据选型）

**Codegen**

- [ ] 树结构指针追逐、分支密度（定性，解释 cache miss）

---

### Week 6：unordered_map/unordered_set（rehash、load factor、allocator、footprint）

**Reading**

- C++SL：unordered（rehash、reserve、load factor、失效规则）
- STL-SCA：hashtable（separate chaining）思想与 rehash 动机

**Labs（语义线）**

- [ ] rehash 触发条件与失效复现（迭代器/引用）
- [ ] `reserve/max_load_factor` 行为边界实验

**Bench（性能线）**

- [ ] insert/find：不同 load factor 下吞吐与尾延迟对比
- [ ] allocator 对抖动影响（优先 pool/free-list；未就绪则至少 CountingAllocator 观测）
  - 备注：LinearAdapter 仅用于“一次性构建、不删除”

**必做增强：Memory Footprint**

- [ ] 观测 bucket+node 内存占用量级（形成结论与选型规则）

**Codegen**

- [ ] hash 计算、桶定位、链表遍历热点（定性）

---

### Week 7：heap/priority_queue（事件驱动常用结构）

**Reading**

- C++SL：heap 算法与容器适配器
- STL-SCA：heap 实现与复杂度推导

**Labs（语义线）**

- [ ] 定时器/事件队列：push/pop/更新优先级的语义边界（需要索引则写清）

**Bench（性能线）**

- [ ] `priority_queue` vs `multiset`：频繁 push/pop/更新对比

**Codegen**

- [ ] 堆调整循环的内联/分支（定性）

---

### Week 8：核心算法（remove/partition/sort/nth_element）与分布敏感性

**Reading**

- C++SL：算法分类；remove-erase；稳定性等
- STL-SCA：introsort 思想；分派与优化点

**Labs（语义线）**

- [ ] remove-erase 惯用法：不同容器差异与正确姿势
- [ ] stable_sort vs sort：稳定性差异复现

**Bench（性能线）**

- [ ] 数据分布：随机/几乎有序/大量重复 → sort 家族表现对比

**Codegen**

- [ ] 比较器内联、分支预测（定性）

---

### Week 9：迭代器、traits、tag dispatch（泛型编程地基）

**Reading**

- C++SL：iterator_traits、迭代器类别
- STL-SCA：traits 萃取与 tag dispatch 经典写法

**Labs（语义线）**

- [ ] `my_advance/my_distance`：
  - random access O(1)
  - forward/input 线性
- [ ] 指针迭代器 traits 偏特化验证

**Bench（性能线）**

- [ ] 同一接口在不同 iterator 类别下的复杂度差异（建立直觉）

**Codegen**

- [ ] 分派到不同实现路径（不追求向量化）

---

### Week 10：Allocator 深入 + 强异常保证 + 结业整合

**Reading**

- C++SL：allocator 使用约束与异常安全（强保证）
- STL-SCA：空间配置器思想（经典 vs 现代对照）

**Labs（语义线）**

- [ ] 事务式更新：build-new + swap commit（强保证）
- [ ] “会抛异常的元素类型”验证不变式（强/基本保证差异）

**Bench（性能线）**

- [ ] 默认 allocator vs pool allocator：小对象高频场景抖动对比（tail-latency 导向）

**交付**

- [ ] `docs/week10.md`：规则库索引（串联前 9 周规则）

---

## 5. Capstone（持续）

三选一（或你指定）：

1. **L2 订单簿**（map/unordered 混合）：档位有序 + 订单索引
2. **行情缓存**（vector/deque + 环形缓冲）：连续内存与批处理优化
3. **事件队列**（heap + 取消/去重索引）：定时器与任务调度

### Capstone 交付要求

- [ ] 选型表：为何不用替代容器（证据：bench + 失效规则 + footprint）
- [ ] 抖动来源分析：allocator、rehash、扩容、cache miss 哪个是主因
- [ ] 可复现实验脚本：一键跑 tests/bench/codegen（不要求 CI，但要可复现）

---

## 6. “过时 vs 经典 vs 现代对应物”对照表（每周必做）

每周至少写 1–2 行，优先覆盖：

- **RVO/NRVO / Copy elision**：旧式“为避免拷贝”的技巧在现代可能不必要
- **Move semantics**：SGI 技巧 vs 现代 `std::move` / `is_trivially_copyable`
- **C++17 node handle（extract/merge）**：关联容器节点转移、无需重分配（作为现代对应物记录）
- **（可选）PMR**：作为未来扩展，不进入主线依赖

---

## 7. 计划确认点（当前阶段）

当前仍处于“确认计划”阶段；在你明确回复 **“计划通过”** 之前，不进入 Phase 0 执行与落地。

你可用以下两种回复之一收口：

- **“计划通过”**
- **指出需要修改的 1–2 个点**（例如：某周顺序、Week6 footprint 是否必做、capstone 选题）
