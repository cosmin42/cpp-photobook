#pragma once

#include <thread_pool/thread_pool.h>

#include <pb/ImageOperations.h>
#include <pb/MediaMap.h>
#include <pb/Project.h>

namespace PB {
class ResizeTask final {
public:
  ResizeTask() = delete;
  explicit ResizeTask(Path fullSizePath, Path smallThumbnailOutputPath,
                      Path mediumThumbnailOutputPath, unsigned totalTaskCount,
                      std::function<void()> onFinish, int screenWidth,
                      int screenHeight);
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
};

class ThumbnailsProcessor final {
public:
  explicit ThumbnailsProcessor(std::pair<int, int> size);
  ~ThumbnailsProcessor();

  void provideProjectDetails(ProjectDetails const &);

  void generateThumbnails(
      std::vector<Path> mediaMap, std::string groupIdentifier,
      std::function<void(Path, Path, Path, int)> onThumbnailWritten);

private:
  std::pair<Path, Path>                      assembleOutputPaths(int         index,
                                                                 std::string groupIdentifier);
  ProjectDetails                             mProjectDetails;
  dp::thread_pool<std::function<void(void)>> mResizePool;
  std::vector<std::future<void>>             mFutures;
  std::function<void(Path, Path, Path, int)> mThumbnailWritten;
  int                                        mScreenWidth;
  int                                        mScreenHeight;
};
} // namespace PB