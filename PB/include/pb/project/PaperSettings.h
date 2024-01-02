#pragma once

#include <pb/Enums.h>
#include <pb/util/Util.h>

namespace PB {
struct PaperSettings {
  PaperType type;
  int       ppi;
  int       width;
  int       height;
};
} // namespace PB

bool operator==(PB::PaperSettings const &lhs, PB::PaperSettings const &rhs);