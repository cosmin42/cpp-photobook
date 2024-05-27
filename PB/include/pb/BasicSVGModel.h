#pragma once

#include <inja/inja.hpp>

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

    for (auto i = 0; i < imagePaths.size(); ++i)
    {
      data[std::to_string(i) + "_image"] = imagePaths.at(i).string();
    }

    return data;
  }
};
} // namespace PB
