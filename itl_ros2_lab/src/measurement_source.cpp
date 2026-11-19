#include "itl_ros2_lab/measurement_source.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace itl_ros2_lab
{
namespace
{

// 循环发布的样例。故意包含空数组、含 0 的数组、单元素数组和重复值数组，
// 便于在第三阶段逐项验证“每组输入对应的输出”。
const std::vector<std::vector<double>> kSamples = {
  {40.0, 60.0, 80.0},   // 期望均值 60
  {},                  // 空数组：mean_node 不应发布
  {0.0, 10.0, 20.0},    // 期望均值 10；0 是合法长度
  {5.0},                // 期望均值 5
  {7.5, 7.5, 7.5, 7.5} // 期望均值 7.5
};

std::string formatLengths(const std::vector<double> & lengths)
{
  std::ostringstream oss;
  oss << '[';
  for (std::size_t i = 0; i < lengths.size(); ++i) {
    if (i != 0) {
      oss << ", ";
    }
    oss << lengths[i];
  }
  oss << ']';
  return oss.str();
}

}  // namespace

MeasurementSource::MeasurementSource()
: Node("measurement_source"), index_(0)
{
  // 话题名和消息类型必须和 mean_node 完全一致。
  publisher_ = this->create_publisher<example_interfaces::msg::Float64MultiArray>(
    "/lengths", 10);

  // 每秒触发一次发布回调。
  timer_ = this->create_wall_timer(
    std::chrono::seconds(1),
    [this]() {this->publishNext();});

  // 启动时立即发第一组，避免刚打开 ros2 topic echo 时要等一秒。
  this->publishNext();

  RCLCPP_INFO(this->get_logger(), "measurement_source started, publishing to /lengths");
}

void MeasurementSource::publishNext()
{
  const std::size_t sample_index = index_ % kSamples.size();
  const auto & lengths = kSamples[sample_index];

  auto message = std::make_unique<example_interfaces::msg::Float64MultiArray>();

  // data 是真正要传输的长度数组。
  message->data = lengths;

  // layout 对本题计算无影响，但填上一个一维描述更规范。
  message->layout.dim.resize(1);
  message->layout.dim[0].label = "lengths";
  message->layout.dim[0].size = static_cast<std::uint32_t>(lengths.size());
  message->layout.dim[0].stride = static_cast<std::uint32_t>(lengths.size());

  RCLCPP_INFO(
    this->get_logger(), "Publishing group %zu to /lengths: %s",
    sample_index + 1, formatLengths(lengths).c_str());

  // 使用 move 发布，避免额外复制整条消息。
  publisher_->publish(std::move(message));
  ++index_;
}

}  // namespace itl_ros2_lab

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<itl_ros2_lab::MeasurementSource>());
  rclcpp::shutdown();
  return 0;
}
