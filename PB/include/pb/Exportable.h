#pragma once

#include <string>

#include <pb/util/Traits.h>

namespace PB {
class Exportable {
public:
  void configureExport(std::string name, Path destination,
                       std::vector<Path> images)
  {
    mName = name;
    mDestination = destination;
    std::copy(images.begin(), images.end(), mImages.begin());
  }

protected:
  std::string       mName;
  Path              mDestination;
  std::vector<Path> mImages;
};
} // namespace PB