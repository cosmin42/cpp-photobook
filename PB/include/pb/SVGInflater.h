#pragma once

#include <optional>

#include <pb/BasicSVGModel.h>
#include <pb/util/Traits.h>

namespace PB {
class SVGInflater {
public:
  std::optional<std::string> inflate(BasicSVGModel svgModel, Path svgFile);
private:
};
} // namespace PB
