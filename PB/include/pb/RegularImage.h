#pragma once

#include <pb/PreprocessedImage.h>
#include <pb/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(Thumbnails thumbnails) : mThumbnails(thumbnails) {}

  RegularImage &buildFullImage(Path path)
  {
    mThumbnails.fullPath = path;
    return *this;
  }

  RegularImage &buildMediumImage(Path path)
  {
    mThumbnails.mediumThumbnail = path;
    return *this;
  }

  RegularImage &buildSmallImage(Path path)
  {
    mThumbnails.smallThumbnail = path;
    return *this;
  }

  cv::Mat          fullImage() override { return cv::Mat(); }
  cv::Mat          mediumImage() override { return cv::Mat(); }
  cv::Mat          smallImage() override { return cv::Mat(); }
  VirtualImageType type() const override { return VirtualImageType::Regular; }

  Thumbnails const &thumbnails() const { return mThumbnails; }

  void setThumbnail(Thumbnails thumbnails) { mThumbnails = thumbnails; }

private:
  Thumbnails mThumbnails;
};
} // namespace PB