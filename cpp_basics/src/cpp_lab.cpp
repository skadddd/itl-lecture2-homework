#include "itl_cpp_basics/length_stats.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace
{

bool g_all_passed = true;

void report(bool condition, const std::string & case_name)
{
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << case_name << '\n';
  if (!condition) {
    g_all_passed = false;
  }
}

// 浮点数不能用 == 直接比较，留一个很小的容差。
bool nearlyEqual(double actual, double expected)
{
  return std::fabs(actual - expected) <= 1e-9;
}

void testMeanLength()
{
  using itl_cpp_basics::meanLength;

  // 讲义例子：40、60、80 的均值应为 60。
  const auto normal = meanLength(std::vector<double>{40.0, 60.0, 80.0});
  report(normal.has_value() && nearlyEqual(*normal, 60.0), "meanLength: {40, 60, 80} -> 60");

  // 空数组必须返回 nullopt，而不是 0 或 NaN。
  const auto empty = meanLength(std::vector<double>{});
  report(!empty.has_value(), "meanLength: {} -> nullopt");

  // 0 是合法的像素值：含 0 的正常数组仍能算出正确均值。
  const auto withZero = meanLength(std::vector<double>{0.0, 10.0, 20.0});
  report(withZero.has_value() && nearlyEqual(*withZero, 10.0), "meanLength: {0, 10, 20} -> 10");

  // 单元素数组：均值等于它本身。
  const auto single = meanLength(std::vector<double>{5.0});
  report(single.has_value() && nearlyEqual(*single, 5.0), "meanLength: {5} -> 5");
}

void testCountAtLeast()
{
  using itl_cpp_basics::countAtLeast;

  // 60 恰好等于阈值，必须计入；80 也满足，因此结果是 2。
  report(countAtLeast(std::vector<double>{40.0, 60.0, 80.0}, 60.0) == 2,
    "countAtLeast: {40, 60, 80}, threshold=60 -> 2");

  // 空数组里没有任何元素满足条件。
  report(countAtLeast(std::vector<double>{}, 60.0) == 0,
    "countAtLeast: {}, threshold=60 -> 0");

  // 专门验证“恰好等于阈值”以及略小、略大的边界。
  report(countAtLeast(std::vector<double>{59.999, 60.0, 60.001}, 60.0) == 2,
    "countAtLeast: {59.999, 60.0, 60.001}, threshold=60 -> 2");

  // 阈值为 0 时，等于 0 的元素也要计入。
  report(countAtLeast(std::vector<double>{0.0, 0.0, -1.0}, 0.0) == 2,
    "countAtLeast: {0, 0, -1}, threshold=0 -> 2");
}

}  // namespace

int main()
{
  std::cout << "=== itl_cpp_basics tests ===\n";
  testMeanLength();
  testCountAtLeast();

  if (!g_all_passed) {
    std::cerr << "Some tests FAILED.\n";
    return 1;  // 测试失败必须返回非零值。
  }

  std::cout << "All tests PASSED.\n";
  return 0;
}
