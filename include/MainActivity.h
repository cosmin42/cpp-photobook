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

  void onFinished(Path rootPath) override {}
  void onStopped() override {}
  void onStarted() override {}
  void onPaused() override {}
  void onResumed() override {}

  void onUnstagedImageAdded(Path rootPath, Path fullPath,
                            Path mediumPath, Path smallPath,
                            int position) override
  {
  }

  void post(std::function<void()>) override {}

  void onProgressUpdate(Path, int, int) override {}
  void onError(PBDev::Error error) override {}

  void onAddingUnstagedImagePlaceholder(unsigned size) override {}

  void onMappingFinished(Path) override {}
};

class MainActivity final {
public:
  MainActivity() : mPhotoBook(Path(".")) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  ConsolePhotobookListener mPhotoBookListener;
  PB::Photobook            mPhotoBook;
};
} // namespace BL