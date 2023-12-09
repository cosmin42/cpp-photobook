#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Config.h>
#include <pb/util/Observable.h>

namespace PB {

struct ImageResources {
  Path full;
  Path medium;
  Path small;
};

class VirtualImage {
public:
  virtual VirtualImageType type() const = 0;

  void setFullSizePath(Path path) { mResources.full = path; }
  void setMediumSizePath(Path path) { mResources.medium = path; }
  void setSmallSizePath(Path path) { mResources.small = path; }

  void setSizePath(Path fullSizePath, Path mediumSizePath = Path(),
                   Path smallSizePath = Path())
  {
    mResources.full = fullSizePath;
    mResources.medium = mediumSizePath;
    mResources.small = smallSizePath;
  }

  ImageResources resources() const { return mResources; }

private:
  ImageResources mResources = {Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE),
                               Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE),
                               Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE)};
};
} // namespace PB