#pragma once

#include <thread_pool/thread_pool.h>

#include <pb/ImageOperations.h>
#include <pb/Project.h>

namespace PB {
class ResizeTask final {
public:
  ResizeTask() = delete;
  explicit ResizeTask(Path fullSizePath, Path smallThumbnailOutputPath,
                      Path mediumThumbnailOutputPath, unsigned totalTaskCount,
                      std::function<void()> onFinish, int screenWidth,
                      int screenHeight, std::stop_token stopToken);
  ~ResizeTask() = default;

  void operator()() const;

private:
  Path                  mFullSizePath;
  Path                  mSmallThumbnailOutputPath;
  Path                  mMediumThumbnailOutputPath;
  unsigned              mTotalTaskCount;
  std::function<void()> mFinish;
  int                   mScreenWidth;
  int                   mScreenHeight;
  std::stop_token       mStopToken;
};

class ThumbnailsProcessor final {
public:
  explicit ThumbnailsProcessor(std::pair<int, int> size);
  ~ThumbnailsProcessor();

  void provideProjectDetails(ProjectSnapshot const &);

  void generateThumbnails(
      std::vector<Path> mediaMap, std::string groupIdentifier,
      std::function<void(Path, Path, Path, int)> onThumbnailWritten);

  void halt();

private:
  std::pair<Path, Path>                      assembleOutputPaths(int         index,
                                                                 std::string groupIdentifier);
  ProjectSnapshot                            mProjectDetails;
  dp::thread_pool<std::function<void(void)>> mResizePool;
  std::vector<std::future<void>>             mFutures;
  std::function<void(Path, Path, Path, int)> mThumbnailWritten;
  int                                        mScreenWidth;
  int                                        mScreenHeight;
  std::vector<std::stop_source> mStopSources;
};
} // namespace PB