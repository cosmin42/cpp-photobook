#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Config.h>

namespace PB {
class VirtualImage {
public:
  virtual VirtualImageType type() const = 0;

  void setFullSizePath(Path path) { mFullSizePath = path; }
  void setMediumSizePath(Path path) { mMediumSizePath = path; }
  void setSmallSizePath(Path path) { mSmallSizePath = path; }

  void setSizePath(Path fullSizePath, Path mediumSizePath = Path(),
                   Path smallSizePath = Path())
  {
    mFullSizePath = fullSizePath;
    mMediumSizePath = mediumSizePath;
    mSmallSizePath = smallSizePath;
  }

  Path fullSizePath() const { return mFullSizePath; }
  Path mediumSizePath() const { return mMediumSizePath; }
  Path smallSizePath() const { return mSmallSizePath; }

private:
  Path mFullSizePath;
  Path mMediumSizePath;
  Path mSmallSizePath = Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE);
};
} // namespace PB