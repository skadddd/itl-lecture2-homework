#include "itl_ros2_lab/mean_node.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "itl_cpp_basics/length_stats.hpp"

namespace itl_ros2_lab
{

MeanNode::MeanNode()
: Node("mean_node")
{
  // 输出话题：Float64，只发布有定义的均值。
  publisher_ = this->create_publisher<example_interfaces::msg::Float64>(
    "/mean_length", 10);

  // 订阅者保存在成员变量中；回调只登记，真正执行由 spin 驱动。
  subscription_ = this->create_subscription<example_interfaces::msg::Float64MultiArray>(
    "/lengths", 10,
    [this](const example_interfaces::msg::Float64MultiArray::SharedPtr message) {
      this->onLengths(message);
    });

  RCLCPP_INFO(this->get_logger(), "mean_node started, waiting for /lengths");
}

void MeanNode::onLengths(const example_interfaces::msg::Float64MultiArray::SharedPtr message)
{
  const std::vector<double> & lengths = message->data;

  // 通信节点只负责“收消息、调函数、发结果”，不在这里重写均值算法。
  const auto mean = itl_cpp_basics::meanLength(lengths);

  if (!mean.has_value()) {
    // 空数组没有均值：只记录日志。
    // 不发布 0，也不把上一次的均值再发一遍——没有结果就不要制造结果。
    RCLCPP_WARN(
      this->get_logger(),
      "Received empty array on /lengths; no mean published (not 0, not stale).");
    return;
  }

  auto output = std::make_unique<example_interfaces::msg::Float64>();
  output->data = *mean;

  RCLCPP_INFO(
    this->get_logger(), "Computed mean = %.6f from %zu length(s); publishing /mean_length",
    *mean, lengths.size());

  publisher_->publish(std::move(output));
}

}  // namespace itl_ros2_lab

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<itl_ros2_lab::MeanNode>());
  rclcpp::shutdown();
  return 0;
}
