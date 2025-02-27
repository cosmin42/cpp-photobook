#pragma once

#include <pb/entities/LutIconInfo.h>
#include <pb/infra/Error.h>

namespace PB {
class NoirListener {
public:
  virtual ~NoirListener() = default;
  virtual void onNoirLutAdded(LutIconInfo iconInfo) = 0;
  virtual void onNoirError(PBDev::Error) = 0;
};
} // namespace PB
