#pragma once

#include <hpdf.h>

#include <pb/util/Traits.h>

namespace PB {
class Pdf final {
public:
  void exportImages(Path dest, std::vector<Path> images);
};
} // namespace PB