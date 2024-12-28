#pragma once

#include <pb/Enums.h>
#include <pb/util/Traits.h>

namespace PB {

class GenericImage {
public:
  static std::string defaultHash();

  static void configureProjectPath(const Path &projectPath)
  {
    sProjectPath = projectPath;
  }

  explicit GenericImage(std::string hash);
  virtual ~GenericImage() = default;

  Path full() const;
  Path medium() const;
  Path smaLL() const;

  std::string hash() const { return mHash; }

  virtual ImageType type() const = 0;

private:
  static Path sProjectPath;
  std::string mHash;

  std::string imageName() const;
};

typedef std::shared_ptr<GenericImage>             GenericImagePtr;
typedef std::vector<GenericImagePtr>              GenericImagePtrLine;
typedef std::vector<std::vector<GenericImagePtr>> GenericImagePtrMatrix;

} // namespace PB
