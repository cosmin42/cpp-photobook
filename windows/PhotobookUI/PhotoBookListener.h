#pragma once

#include <functional>

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/PhotobookListener.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookUI::implementation {
struct TableContentPage;

class PhotoBookListener : public PB::PhotobookListener {
public:
  explicit PhotoBookListener(TableContentPage &parent);
  void onFinished(PB::Path rootPath) override;
  void onStopped() override;
  void onStarted() override;
  void onPaused() override;
  void onResumed() override;

  void onProgressUpdate(PB::Path rootPath, int progress,
                        int reference) override;
  void onUnstagedImageAdded(PB::Path rootPath, PB::Path fullPath,
                            PB::Path mediumPath, PB::Path smallPath,
                            int position) override;
  void onAddingUnstagedImagePlaceholder(unsigned size) override;

  void onStagedImageAdded(PB::Thumbnails image, int index = -1) override;
  void onStagedImageRemoved(std::vector<int> removedIndexes) override;

  void onMappingFinished(PB::Path rootPath) override;

  void onError(PB::Error error) override;

  void post(std::function<void()> f) override;

private:
  TableContentPage &mParent;
};
} // namespace winrt::PhotobookUI::implementation