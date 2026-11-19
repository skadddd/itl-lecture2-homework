# ITL 算法组第二讲作业：C++ / CMake 与 ROS 2 入门

## 0. 环境检查结论

已只读检查，未修改系统配置、未卸载任何软件：

| 项目 | 实际结果 |
| --- | --- |
| 系统 | Ubuntu 22.04.5 LTS |
| 编译器 | g++ 11.4.0（支持 C++17） |
| 构建工具 | CMake 3.22.1、colcon |
| ROS | ROS 2 Humble（`/opt/ros/humble`） |
| 消息包 | `example_interfaces`（`Float64MultiArray`、`Float64` 可用） |

`build/`、`install/`、`log/`、`.tmp_build/` 均为本地构建/临时产物，已写入 `.gitignore`，**不属于提交内容**。

---

## 1. 工程目录结构

```text
<workspace>/
├── README.md
├── .gitignore
├── cpp_basics/                         # 第一阶段：独立 CMake 工程
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── itl_cpp_basics/
│   │       └── length_stats.hpp        # 函数声明
│   └── src/
│       ├── length_stats.cpp            # 函数定义，编译成库
│       └── cpp_lab.cpp                 # 带 main 的测试程序
└── itl_ros2_lab/                       # 第二阶段：ROS 2 C++ 包
    ├── CMakeLists.txt
    ├── package.xml
    ├── include/
    │   └── itl_ros2_lab/
    │       ├── measurement_source.hpp
    │       └── mean_node.hpp
    └── src/
        ├── measurement_source.cpp      # 发布节点
        └── mean_node.cpp               # 订阅/计算/发布节点
```

ROS 2 包中的 `mean_node` **直接链接第一阶段编译所用的同一份** `length_stats.cpp`，没有重写均值算法。

---

## 2. 构建、测试与运行命令

以下 `<workspace>` 指本 README 所在目录。

### 2.1 每个终端先执行

```bash
source /opt/ros/humble/setup.bash
export ROS_LOCALHOST_ONLY=1
cd <workspace>
```

### 2.2 第一阶段：独立 C++ 库与测试

```bash
cd cpp_basics
cmake -S . -B build
cmake --build build -j2
./build/cpp_lab
ctest --test-dir build --output-on-failure
```

实际测试输出：

```text
=== itl_cpp_basics tests ===
[PASS] meanLength: {40, 60, 80} -> 60
[PASS] meanLength: {} -> nullopt
[PASS] meanLength: {0, 10, 20} -> 10
[PASS] meanLength: {5} -> 5
[PASS] countAtLeast: {40, 60, 80}, threshold=60 -> 2
[PASS] countAtLeast: {}, threshold=60 -> 0
[PASS] countAtLeast: {59.999, 60.0, 60.001}, threshold=60 -> 2
[PASS] countAtLeast: {0, 0, -1}, threshold=0 -> 2
All tests PASSED.
```

CTest 结果：`100% tests passed, 0 tests failed out of 1`。

### 2.3 第二阶段：ROS 2 包

```bash
cd <workspace>
mkdir -p .tmp_build
export TMPDIR="$PWD/.tmp_build" && export TEMP="$TMPDIR" && export TMP="$TMPDIR"
colcon build --packages-select itl_ros2_lab --cmake-args -DPython3_EXECUTABLE=/usr/bin/python3
source install/setup.bash
```

说明：

- `-DPython3_EXECUTABLE=/usr/bin/python3` 是为了让 CMake 使用系统 Python 3.10；本机 Python 3.13 环境里没有 ROS Humble 需要的 `catkin_pkg`。
- `TMPDIR` 是为了绕开本机 `/tmp` 只有 10 MB、编译大文件时会报“设备上没有空间”的限制。
- 在普通 Ubuntu 22.04 + ROS Humble 环境中，这两项通常不是必需的，可直接 `colcon build --packages-select itl_ros2_lab`。

### 2.4 运行两个节点

终端 A：

```bash
source /opt/ros/humble/setup.bash
cd <workspace>
source install/setup.bash
export ROS_LOCALHOST_ONLY=1
ros2 run itl_ros2_lab measurement_source
```

终端 B：

```bash
source /opt/ros/humble/setup.bash
cd <workspace>
source install/setup.bash
export ROS_LOCALHOST_ONLY=1
ros2 run itl_ros2_lab mean_node
```

检查通信：

```bash
ros2 node list
ros2 topic list -t
ros2 topic type /lengths
ros2 topic type /mean_length
ros2 topic echo /lengths --once
ros2 topic echo /mean_length --once
```

> 在本次自动化工作区里，分两次工具调用启动的节点能互相“看见”话题，但 DDS 数据不通；用 ROS 2 官方 `talker/listener` 复核后确认是沙箱隔离，不是代码问题。把两个节点放在同一个终端会话/同一条命令中启动即正常。真实 Ubuntu 两个终端不受此限制。

---

## 3. 节点名、话题名和消息类型

| 项目 | 名称 | 类型/说明 |
| --- | --- | --- |
| 节点 1 | `/measurement_source` | 每秒发布一组长度，5 组循环 |
| 节点 2 | `/mean_node` | 订阅长度、调用 `meanLength`、发布均值 |
| 输入话题 | `/lengths` | `example_interfaces/msg/Float64MultiArray` |
| 输出话题 | `/mean_length` | `example_interfaces/msg/Float64` |

实际检查结果：

```text
$ ros2 node list
/mean_node
/measurement_source

$ ros2 topic list -t
/lengths [example_interfaces/msg/Float64MultiArray]
/mean_length [example_interfaces/msg/Float64]

$ ros2 topic type /lengths
example_interfaces/msg/Float64MultiArray

$ ros2 topic type /mean_length
example_interfaces/msg/Float64
```

`ros2 topic echo /mean_length --once` 实际输出示例：

```text
data: 5.0
---
```

---

## 4. 每组输入对应的输出

`measurement_source` 循环发布以下 5 组数据：

| 组 | `/lengths` 中的 data | `mean_node` 行为 | `/mean_length` |
| --- | --- | --- | --- |
| 1 | `[40.0, 60.0, 80.0]` | 计算均值 | `data: 60.0` |
| 2 | `[]` | 打 WARN 日志，**不发布** | 无消息 |
| 3 | `[0.0, 10.0, 20.0]` | 计算均值 | `data: 10.0` |
| 4 | `[5.0]` | 计算均值 | `data: 5.0` |
| 5 | `[7.5, 7.5, 7.5, 7.5]` | 计算均值 | `data: 7.5` |

之后从第 1 组继续循环。

真实 `mean_node` 日志节选：

```text
[INFO] [1795088591.456806080] [mean_node]: Computed mean = 60.000000 from 3 length(s); publishing /mean_length
[WARN] [1795088592.456896231] [mean_node]: Received empty array on /lengths; no mean published (not 0, not stale).
[INFO] [1795088593.456879813] [mean_node]: Computed mean = 10.000000 from 3 length(s); publishing /mean_length
[INFO] [1795088594.456848692] [mean_node]: Computed mean = 5.000000 from 1 length(s); publishing /mean_length
[INFO] [1795088595.456820421] [mean_node]: Computed mean = 7.500000 from 4 length(s); publishing /mean_length
```

可以看到：第 2 组空数组时只有 WARN，没有 `publishing /mean_length`，因此既没有发布 `0`，也没有把上一轮的 `60.0` 再发一遍。

---

## 5. 关键代码与设计选择

1. **`meanLength` 返回 `std::optional<double>`**  
   空输入返回 `std::nullopt`。返回值类型本身就在表达“可能没有结果”，调用者必须先 `has_value()` 再取值，无法无意中把“没有数据”当成正常数字使用。

2. **参数是 `const std::vector<double>&`**  
   不复制整个数组，也不允许函数修改调用者数据。对一帧图像或大量灯条数据，避免复制很重要。

3. **累加初值写成 `0.0`**  
   `std::accumulate(..., 0.0)` 强制在 `double` 域累加，避免 `2 / 3 == 0` 这类整数除法陷阱。

4. **`countAtLeast` 使用 `>=`**  
   题目是“不小于阈值”，所以恰好等于阈值的元素必须计入；测试中专门覆盖了 `60.0` 与阈值 `60.0` 相等的情况。

5. **ROS 节点用类实现，发布者/订阅者/定时器都是成员变量**  
   它们必须和节点同生命周期。若订阅者只是函数里的局部变量，函数返回后订阅就被销毁，回调永远不会执行。

6. **回调只负责“收消息 → 调函数 → 发结果”**  
   均值算法仍在第一阶段的库里。`spin` 负责在消息到达时调用回调；没有 `spin`，回调不会执行。

7. **空数组在回调里提前 `return`**  
   不构造输出消息，因此不会发布 `0`，也不会复用上一次的均值。没有结果就不要制造结果。

---

## 6. 为什么空数组的均值不能返回 0

- 均值定义是“总和除以元素个数”。空数组的元素个数是 0，数学上该值**没有定义**，不是 0。
- 返回 0 会让调用者无法区分两种情况：
  - “均值恰好是 0”（例如数据 `[-10, 10]` 或长度本身为 0）；
  - “根本没有数据”（例如本帧没有检测到灯条）。
- 在机器人系统里，这两种含义完全不同：前者可能是合法目标，后者意味着“本帧没看到目标”。把“没有数据”伪装成 0，会让下游控制、跟踪或日志分析产生难以追查的错误。
- 返回 `NaN` 也不行：`NaN` 不会让程序停止，而会沿后续计算一路传播，最后在云台角度等更远的地方才暴露问题。
- 因此用 `std::optional<double>` / `std::nullopt` 把“无结果”显式写进接口，是最清晰、最不容易误用的表达。

---

## 7. 遇到的问题和解决方案

1. **CMake 链接签名冲突**  
   报错：`The keyword signature for target_link_libraries has already been used...`  
   原因：`ament_target_dependencies` 内部使用 plain 签名，而这里先用了 `PRIVATE` 关键字签名。  
   解决：把 `target_link_libraries(mean_node PRIVATE itl_cpp_basics)` 改为 plain 形式 `target_link_libraries(mean_node itl_cpp_basics)`。

2. **CMake 找不到 `catkin_pkg`**  
   报错：`ModuleNotFoundError: No module named 'catkin_pkg'`  
   原因：环境 PATH 中 Python 3.13 排在前面，但 ROS Humble 的 `catkin_pkg` 安装在系统 Python 3.10（`/usr/lib/python3/dist-packages`）。  
   解决：构建时传入 `--cmake-args -DPython3_EXECUTABLE=/usr/bin/python3`。未修改系统默认 Python，也未安装/卸载软件。

3. **`/tmp` 空间不足导致编译失败**  
   报错：`error writing to /tmp/ccho1CvU.s: 设备上没有空间`  
   原因：本机 `/tmp` 是仅 10 MB 的 tmpfs。  
   解决：创建工作区内的 `.tmp_build`，构建时设置 `TMPDIR/TEMP/TMP` 指向它。

4. **沙箱中两个独立工具调用启动的节点收不到消息**  
   现象：`ros2 topic list` 能看到两个话题，但 `ros2 topic echo` 也收不到数据；用官方 `talker/listener` 在同一命令中测试正常。  
   结论：是自动化沙箱的进程隔离，不是代码、话题名或 QoS 问题。把两个节点放在同一个终端会话中启动后，`ros2 node list`、`ros2 topic echo` 和均值计算全部正常。

5. **`ros2 pkg create` 的 license 警告**  
   生成时 `package.xml` 的 license 是 `TODO`；已改为 `MIT`，重新构建通过。

---

## 8. 需要掌握的知识点与答辩准备

### 必背知识点

- C++ 类型、单位约定、`std::vector`、值传递 vs 引用传递、`const` 引用。
- `std::optional` / `std::nullopt`：如何表达“没有结果”。
- 头文件放声明、`src/` 放定义、编译与链接的区别、库与目标的概念。
- CMake 目标：`add_library`、`add_executable`、`target_include_directories`、`target_link_libraries`。
- ROS 2 三要素：节点、话题、消息；`example_interfaces` 的使用。
- 回调与 `spin`：订阅者必须是成员变量，回调由 `spin` 驱动。
- QoS 相容性、完整话题名、环境加载一致性等排错顺序。
- `ROS_LOCALHOST_ONLY=1` 的作用：避免收到同一局域网其他人的节点数据。

### 可能的答辩问题

1. `meanLength` 为什么不用 `double` 而用 `std::optional<double>`？返回 `0` 或 `NaN` 有什么问题？
2. 参数为什么写成 `const std::vector<double>&`？如果写成 `std::vector<double>` 会有什么代价？
3. 空数组时 `mean_node` 为什么既不发 `0` 也不发上一次的结果？
4. 编译错误和链接错误分别怎么判断？`undefined reference` 属于哪个阶段？
5. `add_library` 和 `add_executable` 分别做了什么？`target_link_libraries` 解决什么问题？
6. ROS 2 中话题名和消息类型都一致，为什么仍可能收不到消息？请说出 QoS、spin、变量生命周期等原因。
7. 为什么订阅者要定义为成员变量？写成回调函数里的局部变量会怎样？
8. `countAtLeast` 里为什么用 `>=` 而不是 `>`？如何验证“恰好等于阈值”的情况？
9. `std::accumulate` 的初值为什么写 `0.0` 而不是 `0`？
10. 如果现场把输入话题名从 `/lengths` 改成 `/new_lengths`，需要改哪些文件？如果修改均值规则（例如改成中位数），应该改哪里？为什么？
