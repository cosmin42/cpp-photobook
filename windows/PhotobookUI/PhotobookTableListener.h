#pragma once

#include <functional>

#include <pb/DataManager.h>
#include <pb/util/Error.h>
#include <pb/PhotobookListener.h>
#include <pb/util/Traits.h>

namespace winrt::PhotobookUI::implementation {
struct TableContentPage;

class PhotobookTableListener : public PB::PhotobookListener {
public:
  explicit PhotobookTableListener(TableContentPage &parent);

  void onProgressUpdate(Path rootPath, int progress,
                        int reference) override;
  void onExportProgressUpdate(int, int) override;

  void onExportFinished() override;

  void onUnstagedImageAdded(Path rootPath, Path fullPath,
                            Path mediumPath, Path smallPath,
                            int position) override;

  void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                          int index = -1) override;
  void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override;

  void onMappingFinished(Path rootPath) override;

  void onError(PBDev::Error error) override;

  void post(std::function<void()> f) override;

private:
  TableContentPage &mParent;
};
} // namespace winrt::PhotobookUI::implementation