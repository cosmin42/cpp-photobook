#pragma once

#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include <pb/Config.h>
#include <pb/ImageMonitor.h>
#include <pb/StagedImages.h>
#include <pb/image/VirtualImage.h>
#include <pb/util/IteratorWithState.h>
#include <pb/util/Observable.h>

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