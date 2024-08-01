#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Config.h>
#include <pb/util/Util.h>

#include <pb/Platform.h>

namespace PB {

struct ImageResources {
  Path full;
  Path medium;
  Path small;
  unsigned  width = 438;
  unsigned height = 310;
};

class VirtualImage {
public:
  VirtualImage(ImageResources imageResources) : mFrontend(imageResources) {}

  virtual ~VirtualImage() = default;

  virtual VirtualImageType type() const = 0;

  virtual std::vector<Path> resources() const = 0;

  virtual Path keyPath() const = 0;

  void setFullSizePath(Path path) { mFrontend.full = path; }
  void setMediumSizePath(Path path) { mFrontend.medium = path; }
  void setSmallSizePath(Path path) { mFrontend.small = path; }

  void setSizePath(Path fullSizePath, Path mediumSizePath = Path(),
                   Path smallSizePath = Path())
  {
    mFrontend.full = fullSizePath;
    mFrontend.medium = mediumSizePath;
    mFrontend.small = smallSizePath;
  }

  ImageResources frontend() const { return mFrontend; }

  bool processed() const { return mProcessed; }

  void finishProcessing() { mProcessed = true; }

protected:
  ImageResources mFrontend;

  bool mProcessed = false;
};
} // namespace PB