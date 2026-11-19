cmake_minimum_required(VERSION 3.8)
project(itl_ros2_lab)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 与第一阶段保持一致，使用 C++17。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED)

# 直接复用第一阶段的函数库源码，保证 ROS 节点调用的就是已测试过的 meanLength。
add_library(itl_cpp_basics
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp_basics/src/length_stats.cpp
)
target_include_directories(itl_cpp_basics
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../cpp_basics/include
)

# 节点 1：measurement_source
add_executable(measurement_source
  src/measurement_source.cpp
)
target_include_directories(measurement_source PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
ament_target_dependencies(measurement_source rclcpp example_interfaces)

# 节点 2：mean_node，链接第一阶段实现的库。
add_executable(mean_node
  src/mean_node.cpp
)
target_include_directories(mean_node PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp_basics/include
)
# 这里使用 plain 签名，和 ament_target_dependencies 内部调用保持一致。
target_link_libraries(mean_node itl_cpp_basics)
ament_target_dependencies(mean_node rclcpp example_interfaces)

install(TARGETS
  measurement_source
  mean_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
