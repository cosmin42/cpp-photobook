#pragma once

#include <functional>

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookUI::implementation {
struct TableContentPage;

class PhotoBookListener {
public:
  explicit PhotoBookListener(TableContentPage &parent);
  void onFinished(PB::Path rootPath);
  void onStopped();
  void onStarted();
  void onPaused();
  void onResumed();

  void onProgressUpdate(PB::Path rootPath, int progress, int reference);
  void onUnstagedImageAdded(PB::Path rootPath, PB::Path fullPath,
                            PB::Path mediumPath,
                            PB::Path smallPath, int position);
  void onAddingUnstagedImagePlaceholder(unsigned size);

  void onStagedImageAdded(PB::Thumbnails image, int index = -1);
  void onStagedImageRemoved(std::vector<int> removedIndexes);

  void onMappingFinished(PB::Path rootPath);

  void onError(PB::Error error);

  void post(std::function<void()> f);

private:
  TableContentPage &mParent;
};
} // namespace winrt::PhotobookUI::implementation