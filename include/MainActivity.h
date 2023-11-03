#pragma once

#include <pb/PhotoBook.h>
#include <pb/PhotobookListener.h>

namespace BL {

class ConsolePhotobookListener final : public PB::PhotobookListener {
public:
  void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> image, int index = -1) override {}
  void onStagedImageRemoved(std::vector<int> removedIndexes) override {}

  void onExportProgressUpdate(int, int) {}
  void onExportFinished() {}

  void onFinished(PB::Path rootPath) override {}
  void onStopped() override {}
  void onStarted() override {}
  void onPaused() override {}
  void onResumed() override {}

  void onUnstagedImageAdded(PB::Path rootPath, PB::Path fullPath,
                            PB::Path mediumPath, PB::Path smallPath,
                            int position) override
  {
  }

  void post(std::function<void()>) override {}

  void onProgressUpdate(PB::Path, int, int) override {}
  void onError(PB::Error error) override {}

  void onAddingUnstagedImagePlaceholder(unsigned size) override {}

  void onMappingFinished(PB::Path) override {}
};

class MainActivity final {
public:
  MainActivity() : mPhotoBook(mPhotoBookListener, PB::Path("."), {0, 0}) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  ConsolePhotobookListener mPhotoBookListener;
  PB::Photobook            mPhotoBook;
};
} // namespace BL