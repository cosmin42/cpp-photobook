#pragma once

#include <pb/Enums.h>
#include <pb/util/Traits.h>

namespace PB {

class GenericImage {
public:
  static std::string defaultHash();

  explicit GenericImage(Path projectPath, std::string hash);
  virtual ~GenericImage() = default;

  Path full() const;
  Path medium() const;
  Path smaLL() const;

  std::string hash() const { return mHash; }

  virtual ImageType type() const = 0;

private:
  Path        mProjectPath;
  std::string mHash;
};

typedef std::shared_ptr<GenericImage> GenericImagePtr;

} // namespace PB
