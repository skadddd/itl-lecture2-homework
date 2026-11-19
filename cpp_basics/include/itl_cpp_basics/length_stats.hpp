#ifndef ITL_CPP_BASICS_LENGTH_STATS_HPP_
#define ITL_CPP_BASICS_LENGTH_STATS_HPP_

#include <cstddef>
#include <optional>
#include <vector>

namespace itl_cpp_basics
{

/**
 * @brief 计算一组长度的算术平均值。
 *
 * 输入使用 const 引用：既不复制整个 vector，也不允许在函数内修改调用者的数据。
 * 空数组没有数学意义上的均值，因此返回 std::nullopt；绝不返回 0 或 NaN，
 * 避免调用者把“没有数据”误当成“均值恰好为 0”。
 *
 * @param lengths 长度数组，单位为像素（调用者需自行保证单位一致）。
 * @return 正常情况返回均值；空数组返回 std::nullopt。
 */
std::optional<double> meanLength(const std::vector<double> & lengths);

/**
 * @brief 统计数组中不小于（>=）给定阈值的元素个数。
 *
 * 使用 >= 而不是 >，因此“恰好等于阈值”的元素会被计入。
 *
 * @param lengths 长度数组。
 * @param threshold 阈值。
 * @return 满足 length >= threshold 的元素数量。
 */
std::size_t countAtLeast(const std::vector<double> & lengths, double threshold);

}  // namespace itl_cpp_basics

#endif  // ITL_CPP_BASICS_LENGTH_STATS_HPP_
