#pragma once

#include <pb/util/Traits.h>

namespace PB {
class GenericImage {
public:
  explicit GenericImage(Path projectPath, std::string hash);
  virtual ~GenericImage() = default;

  Path full() const;
  Path medium() const;
  Path small() const;

private:
  Path        mProjectPath;
  std::string mHash;
};
} // namespace PB
