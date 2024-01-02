#pragma once

#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include <pb/Config.h>
#include <pb/ImageMonitor.h>
#include <pb/StagedImages.h>
#include <pb/image/Image.h>
#include <pb/util/Util.h>

namespace PB {
class ImageViews final {
public:
  ImageMonitor &imageMonitor() { return mImageMonitor; }
  StagedImages &stagedImages() { return mStagedImages; }

private:
  ImageMonitor mImageMonitor;
  StagedImages mStagedImages;
};

} // namespace PB