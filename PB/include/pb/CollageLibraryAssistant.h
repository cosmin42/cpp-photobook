#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#pragma warning(pop)

#include <pb/AspectRatio.h>
#include <pb/SVGInflater.h>
#include <pb/util/Traits.h>

namespace PB {
class CollageLibraryAssistant final {
public:
  explicit CollageLibraryAssistant(Path collageLibraryThumbnailsDirectory,
                                   Path sourceTemplateDirectory);
  ~CollageLibraryAssistant() = default;

  std::vector<Path> createNumberedImages(cv::Size pageSize);

  void createTemplateThumbnail(Path templateFile, AspectRatio aspectRatio, cv::Size pageSize);

private:
  Path createNumberedImage(cv::Size pageSize, unsigned index, std::string name);

  Path        mCollageLibraryThumbnailsDirectory;
  SVGInflater mThumbnailsSVGInflater;

  static constexpr unsigned THUMBNAIL_HEIGHT = 80;
  static constexpr unsigned MAX_NUMBER_OF_PLACEHOLDER_IMAGES = 10;
};
} // namespace PB