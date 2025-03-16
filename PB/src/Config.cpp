#include <pb/Config.h>

#include <exception>
#include <source_location>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PBDev {
void basicAssert(int shouldBetrue, const std::source_location location)
{
  if (!shouldBetrue) {
    std::string message = std::string(location.function_name()) + ":" +
                          std::to_string(location.line());

#ifndef _CLANG_UML_
    spdlog::info("{}\n", message);
#endif
  }
#if defined(_DEBUG)
  assert(shouldBetrue);
#else
  std::terminate();
#endif
}
} // namespace PBDev