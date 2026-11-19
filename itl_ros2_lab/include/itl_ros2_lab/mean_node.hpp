#ifndef ITL_ROS2_LAB_MEAN_NODE_HPP_
#define ITL_ROS2_LAB_MEAN_NODE_HPP_

#include <memory>

#include "example_interfaces/msg/float64.hpp"
#include "example_interfaces/msg/float64_multi_array.hpp"
#include "rclcpp/rclcpp.hpp"

namespace itl_ros2_lab
{

/**
 * @brief 订阅 /lengths，调用第一阶段的 meanLength，发布均值到 /mean_length。
 *
 * 空数组只打日志，不发布任何消息：既不发 0，也不重复上一次的结果。
 */
class MeanNode : public rclcpp::Node
{
public:
  MeanNode();

private:
  /// 收到一组长度后的回调。
  void onLengths(const example_interfaces::msg::Float64MultiArray::SharedPtr message);

  // 订阅者和发布者都必须是成员变量，不能是函数里的局部变量，
  // 否则离开作用域后订阅/发布关系会被销毁。
  rclcpp::Subscription<example_interfaces::msg::Float64MultiArray>::SharedPtr subscription_;
  rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr publisher_;
};

}  // namespace itl_ros2_lab

#endif  // ITL_ROS2_LAB_MEAN_NODE_HPP_
