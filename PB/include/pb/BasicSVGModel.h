#pragma once

#include <inja/inja.hpp>

#include <pb/util/Traits.h>

namespace PB {
struct BasicSVGModel {
  unsigned width;
  unsigned height;

  std::vector<Path> imagePaths;

  inja::json toJson() const
  {
    inja::json data;
    data["width"] = width;
    data["height"] = height;

    for (auto i = 0; i < imagePaths.size(); ++i) {
      data["image_" + std::to_string(i)] = imagePaths.at(i).string();
    }

    return data;
  }
};
} // namespace PB