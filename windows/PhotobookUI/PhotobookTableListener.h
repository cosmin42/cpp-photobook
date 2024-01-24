#pragma once

#include <functional>

#include <pb/DataManager.h>
#include <pb/PhotobookListener.h>
#include <pb/ProgressManager.h>
#include <pb/util/Util.h>

namespace winrt::PhotobookUI::implementation {
struct TableContentPage;

class PhotobookTableListener : public PB::PhotobookListener {
public:
  void configure(TableContentPage *parent);

  void onProjectRead() override;
  void onProjectRenamed() override;
  void onMetadataUpdated() override;
  void onPersistenceError(PBDev::Error) override;

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

  void onProgressUpdate(std::vector<std::string> names,
                        PB::ProgressInfo         definedProgress,
                        PB::ProgressInfo         undefinedProgress) override;

private:
  TableContentPage *mParent = nullptr;
};
} // namespace winrt::PhotobookUI::implementation