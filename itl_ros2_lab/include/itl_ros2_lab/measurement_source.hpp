#ifndef ITL_ROS2_LAB_MEASUREMENT_SOURCE_HPP_
#define ITL_ROS2_LAB_MEASUREMENT_SOURCE_HPP_

#include <cstddef>
#include <memory>
#include <vector>

#include "example_interfaces/msg/float64_multi_array.hpp"
#include "rclcpp/rclcpp.hpp"

namespace itl_ros2_lab
{

/**
 * @brief 每秒向 /lengths 发布一组灯条长度，循环使用多组样例。
 *
 * 节点类继承 rclcpp::Node。发布者和定时器都是成员变量，
 * 从而保证它们和节点具有相同的生命周期。
 */
class MeasurementSource : public rclcpp::Node
{
public:
  MeasurementSource();

private:
  /// 发布下一组数据，并推进循环下标。
  void publishNext();

  rclcpp::Publisher<example_interfaces::msg::Float64MultiArray>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::size_t index_{0};
};

}  // namespace itl_ros2_lab

#endif  // ITL_ROS2_LAB_MEASUREMENT_SOURCE_HPP_
