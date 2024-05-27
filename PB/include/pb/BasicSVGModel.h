#pragma once

#include <inja/inja.hpp>

namespace PB {
struct BasicSVGModel {
  unsigned width;
  unsigned height;

  inja::json toJson()
  {
    inja::json data;
    data["width"] = width;
    data["height"] = height;
    return data;
  }
};
} // namespace PB
