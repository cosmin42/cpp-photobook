// clang-format off
#include "pch.h"
#include "WinConversions.h"
// clang-format on

boost::uuids::uuid WinConversions::toNativeUuid(winrt::guid managedUuid)
{
  boost::uuids::uuid nativeUuid;
  nativeUuid.data[0] = ((managedUuid.Data1 & 0xFF000000) >> 24);
  nativeUuid.data[1] = (uint8_t)((managedUuid.Data1 & 0xFF0000) >> 16);
  nativeUuid.data[2] = (uint8_t)((managedUuid.Data1 & 0xFF00) >> 8);
  nativeUuid.data[3] = (uint8_t)(managedUuid.Data1 & 0xFF);
  nativeUuid.data[4] = (uint8_t)((managedUuid.Data2 & 0xFF00) >> 8);
  nativeUuid.data[5] = (uint8_t)(managedUuid.Data2 & 0xFF);
  nativeUuid.data[6] = (uint8_t)((managedUuid.Data3 & 0xFF00) >> 8);
  nativeUuid.data[7] = (uint8_t)(managedUuid.Data3 & 0xFF);
  for (int i = 0; i < 8; i++) {
    nativeUuid.data[8 + i] = managedUuid.Data4[i];
  }
  return nativeUuid;
}

winrt::guid WinConversions::toManagedGuid(boost::uuids::uuid nativeUuid)
{
  uint32_t data1 = nativeUuid.data[0] << 24 | nativeUuid.data[1] << 16 |
                   nativeUuid.data[2] << 8 | nativeUuid.data[3];
  uint16_t data2 = nativeUuid.data[4] << 8 | nativeUuid.data[5];
  uint16_t data3 = nativeUuid.data[6] << 8 | nativeUuid.data[7];
  GUID     existingGuid = {(unsigned long)data1,
                           data2,
                           data3,
                           {nativeUuid.data[8], nativeUuid.data[9],
                            nativeUuid.data[10], nativeUuid.data[11],
                            nativeUuid.data[12], nativeUuid.data[13],
                            nativeUuid.data[14], nativeUuid.data[15]}};
  return winrt::guid(existingGuid);
}
