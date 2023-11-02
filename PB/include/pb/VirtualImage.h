#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Config.h>
#include <pb/PreprocessedImage.h>

namespace PB {
class VirtualImage {
public:
  VirtualImage(Thumbnails thumbnails) : mThumbnails(thumbnails) {}

  virtual cv::Mat image() = 0;
  virtual Path    path() = 0;

  Thumbnails const &thumbnails() const { return mThumbnails; }

  void setSmall(Path path) { mThumbnails.smallThumbnail = path; }
  void setMedium(Path path) { mThumbnails.mediumThumbnail = path; }
  void setFull(Path path) { mThumbnails.fullPath = path; }

  void setThumbnail(Thumbnails thumbnails) { mThumbnails = thumbnails; }

private:
  Thumbnails mThumbnails;
};
} // namespace PB