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
                      std::function<void()> onFinish);

  void operator()() const;

private:
  Path                  mFullSizePath;
  Path                  mSmallThumbnailOutputPath;
  Path                  mMediumThumbnailOutputPath;
  unsigned              mTotalTaskCount;
  std::function<void()> mFinish;
};

class ThumbnailsProcessor final {
public:
  ThumbnailsProcessor();
  ~ThumbnailsProcessor();

  void provideProjectDetails(ProjectDetails const &);

  void generateThumbnails(
      std::vector<std::filesystem::path>         mediaMap,
      std::function<void(Path, Path, Path, int)> onThumbnailWritten);

private:
  static constexpr const char *sSmallThumbnailPrefix = "thumbnail-small";
  static constexpr const char *sMediumThumbnailPrefix = "thumbnail-medium";
  static constexpr unsigned    sNumberOfThreads = 4;
  std::pair<Path, Path>        assembleOutputPaths(int index);
  ProjectDetails               mProjectDetails;
  dp::thread_pool<std::function<void(void)>> mResizePool;
  std::vector<std::future<void>>             mFutures;
  std::function<void(Path, Path, Path, int)> mThumbnailWritten;
};
} // namespace PB