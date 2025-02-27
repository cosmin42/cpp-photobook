#pragma once

#include <stop_token>

#include <pb/infra/Traits.h>

namespace PB {
class ImportImageTask final {
public:
  ImportImageTask() = delete;
  explicit ImportImageTask(Path fullSizePath, Path medium, Path small,
                           std::function<void(unsigned, unsigned)> onFinish,
                           int screenWidth, int screenHeight,
                           std::stop_token stopToken);
  ~ImportImageTask() = default;

  void operator()() const;

private:
  Path                                    mFullSizePath;
  Path                                    mSmallThumbnailOutputPath;
  Path                                    mMediumThumbnailOutputPath;
  std::function<void(unsigned, unsigned)> mFinish;
  int                                     mScreenWidth;
  int                                     mScreenHeight;
  std::stop_token                         mStopToken;
};
} // namespace PB
