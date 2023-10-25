#pragma once

#include <pb/Enums.h>
#include <pb/util/Traits.h>

namespace PB {
struct PaperSettings {
  PaperType type;
  int       ppi;
  int       width;
  int       height;

  operator Json() const
  {
    Json jsonData;
    jsonData["type"] = (int)type;
    jsonData["ppi"] = ppi;
    jsonData["width"] = width;
    jsonData["height"] = height;
    return jsonData;
  }
};
} // namespace PB