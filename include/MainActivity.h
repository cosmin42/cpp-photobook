#pragma once

#include <pb/PhotoBook.h>
#include <pb/PhotobookListener.h>

namespace BL {

class ConsolePhotobookListener final : public PB::PhotobookListener {
public:
  void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> image,
                          int index = -1) override
  {
  }

  void onExportProgressUpdate(int, int) {}

  void post(std::function<void()>) override {}

  void onError(PBDev::Error error) override {}

  void onMappingFinished(Path) override {}

  virtual void onProjectRead() override {}
  virtual void onProjectRenamed() override {}
  virtual void onMetadataUpdated() override {}
  virtual void onPersistenceError(PBDev::Error) override {}

  virtual void onProgressUpdate(PB::ProgressInfo definedProgress,
                                PB::ProgressInfo undefinedProgress) override
  {
  }
  virtual void onExportFinished() override {}

  virtual void onStagedImageRemoved(std::vector<unsigned>) override {}

  virtual void onMappingStarted(Path path) override {}
  virtual void onMappingAborted(Path path) override {}

  virtual void onImageUpdated(Path root, int row, int index) override {}
};

class MainActivity final {
public:
  MainActivity() : mPhotoBook(".", ".") {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  ConsolePhotobookListener mPhotoBookListener;
  PB::Photobook            mPhotoBook;
};
} // namespace BL