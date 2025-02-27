#pragma once

#include <pb/infra/Traits.h>

namespace PB {
struct CollageTemplateInfo {
  std::string name;
  Path        path;
  unsigned    imageCount;
};
} // namespace PB
