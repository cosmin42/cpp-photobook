#pragma once

#include <thread_pool/thread_pool.h>

#include <pb/image/ImageOperations.h>
#include <pb/project/Project.h>
#include <pb/util/ParallelTaskConsumer.h>

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
  explicit ThumbnailsProcessor();
  ~ThumbnailsProcessor();

  void setScreenSize(std::pair<int, int> size);

  void provideProjectDetails(std::shared_ptr<Project>);

  void generateThumbnails(
      std::vector<Path> mediaMap, std::string groupIdentifier,
      std::function<void(Path, Path, Path, int)> onThumbnailWritten);

  void abort();

private:
  std::pair<Path, Path>                      assembleOutputPaths(int         index,
                                                                 std::string groupIdentifier);
  std::shared_ptr<Project>                   mProject;

  PBDev::ParallelTaskConsumer mParallelTaskConsumer;
  
  std::vector<std::future<void>>             mFutures;
  std::function<void(Path, Path, Path, int)> mThumbnailWritten;
  int                                        mScreenWidth = 0;
  int                                        mScreenHeight = 0;
  std::vector<std::stop_source>              mStopSources;
};
} // namespace PB