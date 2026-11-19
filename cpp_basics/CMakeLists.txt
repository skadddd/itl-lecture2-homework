cmake_minimum_required(VERSION 3.16)

project(itl_cpp_basics LANGUAGES CXX)

# 统一使用 C++17，不使用编译器扩展。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 第一个目标：独立的函数库。
# 头文件放在 include/，定义放在 src/，其他程序只需要包含头文件并链接这个库。
add_library(itl_cpp_basics
  src/length_stats.cpp
)
target_include_directories(itl_cpp_basics
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 打开常用警告，帮助尽早发现类型不匹配等问题。
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(itl_cpp_basics PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 第二个目标：带 main 的测试程序，链接上面的库。
add_executable(cpp_lab
  src/cpp_lab.cpp
)
target_link_libraries(cpp_lab PRIVATE itl_cpp_basics)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(cpp_lab PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 让 ctest 也能跑同一个测试程序。
enable_testing()
add_test(NAME cpp_lab COMMAND cpp_lab)
