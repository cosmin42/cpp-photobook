#include <FileComparator.h>

#include <cassert>
#include <filesystem>

namespace PB {
template <>
std::strong_ordering
CustomComparator::operator()(std::filesystem::path const &a,
                             std::filesystem::path const &b)
{
  assert(std::filesystem::exists(a) && "File doesn't exist.");
  assert(std::filesystem::exists(b) && "File doesn't exist.");
  return std::strong_ordering::greater;
}
} // namespace PB
