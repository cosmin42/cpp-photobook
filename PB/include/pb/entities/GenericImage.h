#pragma once

#include <pb/Enums.h>
#include <pb/infra/Traits.h>

namespace PB {

class GenericImage {
public:
  static std::string defaultHash();

  explicit GenericImage(std::string hash);
  virtual ~GenericImage() = default;

  Path full() const;
  Path medium() const;
  Path smaLL() const;

  std::string hash() const { return mHash; }

  virtual ImageType type() const = 0;

private:
  std::string mHash;

  std::string imageName() const;
};

typedef std::shared_ptr<GenericImage>             GenericImagePtr;
typedef std::vector<GenericImagePtr>              GenericImagePtrLine;
typedef std::vector<std::vector<GenericImagePtr>> GenericImagePtrMatrix;

} // namespace PB
