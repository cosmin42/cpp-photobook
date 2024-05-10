#include <pb/RuntimeUUID.h>

namespace PB {
uint8_t RuntimeUUID::guid[UUID_SIZE] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                                        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

void RuntimeUUID::increment()
{
  for (size_t i = 0; i < UUID_SIZE; ++i) {
    if (guid[i] == 0xFF) {
      guid[i] = 0x0;
    }
    else {
      ++guid[i];
      break;
    }
  }
}

boost::uuids::uuid RuntimeUUID::newUUID()
{
  boost::uuids::uuid u;
  for (size_t i = 0; i < UUID_SIZE; ++i) {
    u.data[i] = guid[i];
  }
  increment();
  return u;
}
} // namespace PB