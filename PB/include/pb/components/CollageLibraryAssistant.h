#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#pragma warning(pop)

#include <pb/components/SVGInflater.h>
#include <pb/entities/AspectRatio.h>
#include <pb/infra/Traits.h>

namespace PB {
class CollageLibraryAssistant final {
public:
  explicit CollageLibraryAssistant(Path outputFolder);
  ~CollageLibraryAssistant() = default;

  std::vector<Path> createNumberedImages(cv::Size pageSize);

  Path createTemplateThumbnail(std::vector<Path> numberedImages,
                               Path templateFile, AspectRatio aspectRatio,
                               cv::Size pageSize, std::string filename = "");

private:
  Path createNumberedImage(cv::Size pageSize, unsigned index, std::string name);

  SVGInflater mThumbnailsSVGInflater;
  Path        mOutputFolder;
};
} // namespace PB
