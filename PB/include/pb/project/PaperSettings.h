#pragma once

#include <pb/Enums.h>
#include <pb/util/Util.h>

namespace PB {
struct PaperSettings {
  PaperType type = PaperType::A4_Landscape;
  int       ppi = 300;
  int       width = 3508;
  int       height = 2480;
};
} // namespace PB

bool operator==(PB::PaperSettings const &lhs, PB::PaperSettings const &rhs);