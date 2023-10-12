#include <pb/Config.h>

namespace PB {
Context Context::sContext;

void basicAssert(int shouldBetrue)
{
#ifdef _DEBUG
  assert(shouldBetrue);
#else
  (void)shouldBetrue;
#endif
}
} // namespace PB