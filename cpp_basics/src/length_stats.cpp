#include "itl_cpp_basics/length_stats.hpp"

#include <cstddef>
#include <numeric>
#include <optional>
#include <vector>

namespace itl_cpp_basics
{

std::optional<double> meanLength(const std::vector<double> & lengths)
{
  // 第一件事：先问“输入为空会怎样”。空数组的均值未定义，
  // 用 std::nullopt 明确表达“没有结果”，而不是制造一个假的数值。
  if (lengths.empty()) {
    return std::nullopt;
  }

  // 累加初值写成 0.0（double），保证累加和除法都在 double 域进行，
  // 避免整数除法导致 2 / 3 == 0 这类隐蔽错误。
  const double sum = std::accumulate(lengths.begin(), lengths.end(), 0.0);

  // size() 是 size_t，显式转成 double 后再做除法，语义更清晰。
  return sum / static_cast<double>(lengths.size());
}

std::size_t countAtLeast(const std::vector<double> & lengths, double threshold)
{
  std::size_t count = 0;

  // 只读遍历，使用范围 for + const 引用语义（这里元素是 double，按值也廉价，
  // 写成 const double 强调不会修改容器）。
  for (const double length : lengths) {
    // 题目要求“不小于阈值”，所以用 >=；恰好等于阈值的元素必须计入。
    if (length >= threshold) {
      ++count;
    }
  }

  return count;
}

}  // namespace itl_cpp_basics
