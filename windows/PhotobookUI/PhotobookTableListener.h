#pragma once

#include <functional>

#include <pb/DataManager.h>
#include <pb/PhotobookListener.h>
#include <pb/util/Error.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookUI::implementation {
struct TableContentPage;

class PhotobookTableListener : public PB::PhotobookListener {
public:

  void configure(TableContentPage *parent);

  void onProgressUpdate(int progress, int reference) override;
  void onExportProgressUpdate(int, int) override;

  void onExportFinished() override;

  void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                          int index = -1) override;
  void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override;

  void onMappingStarted(Path path) override;
  void onMappingFinished(Path path) override;
  void onMappingAborted(Path path) override;

  void onImageUpdated(Path root, int row, int index) override;

  void onError(PBDev::Error error) override;

  void post(std::function<void()> f) override;

private:
  TableContentPage* mParent = nullptr;
};
} // namespace winrt::PhotobookUI::implementation