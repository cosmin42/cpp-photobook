#include <pb/Config.h>

#include <source_location>

namespace PB {
Context Context::sContext;
}

namespace PBDev
{
void basicAssert(int shouldBetrue, const std::source_location location)
{
  if (!shouldBetrue) {
    std::string message = std::string(location.function_name()) + ":" +
                          std::to_string(location.line());

    PB::printDebug("%s\n", message.c_str());
  }
#ifdef _DEBUG
  assert(shouldBetrue);
#else
  (void)shouldBetrue;
#endif
}
} // namespace PB