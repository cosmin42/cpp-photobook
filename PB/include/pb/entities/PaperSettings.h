#pragma once

#include <string>

#include <magic_enum/magic_enum.hpp>

#include <pb/Enums.h>

namespace PB {
struct PaperSettings {
  PaperType type = PaperType::A4_Landscape;
  int       ppi = 300;
  int       width = 3508;
  int       height = 2480;

  // add to string method
  operator std::string() const
  {
    return std::to_string(width) + "x" + std::to_string(height) + " " +
           std::to_string(ppi) + "ppi " + std::string(magic_enum::enum_name(type));
  }
};

PaperType deserializePaperType(std::string paperType);

} // namespace PB

bool operator==(PB::PaperSettings const &lhs, PB::PaperSettings const &rhs);