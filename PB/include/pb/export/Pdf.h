#pragma once

#include <hpdf.h>

#include <pb/infra/Traits.h>

namespace PB {
class Pdf final {
public:
  void exportImages(std::string name, Path dest, std::vector<Path> images);

private:
  static constexpr int PPI = 72;
  static constexpr int A4_WIDTH = (3508 * 72) / 300;
  static constexpr int A4_HEIGHT = (2480 * 72) / 300;
};
} // namespace PB