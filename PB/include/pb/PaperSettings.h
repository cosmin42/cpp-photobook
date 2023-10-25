#pragma once

#include <pb/Enums.h>

namespace PB {
struct PaperSettings {
  PaperType type;
  int       ppi;
  int       width;
  int       height;
};
} // namespace PB