#pragma once

#include <pb/Enums.h>
#include <pb/util/Util.h>

namespace PB {
struct PaperSettings {
  PaperType type = PaperType::A4_Landscape;
  int       ppi = 72;
  int       width = 842;
  int       height = 595;
};
} // namespace PB

bool operator==(PB::PaperSettings const &lhs, PB::PaperSettings const &rhs);