#pragma once

#include <thread_pool/thread_pool.h>

#include <pb/image/Image.h>
#include <pb/project/Project.h>
#include <pb/tasks/ParallelTaskConsumer.h>

#define SIMULATE_SLOW_THUMBNAILS_PROCESSOR

namespace PB {
class ResizeTask final {
public:
  ResizeTask() = delete;
  explicit ResizeTask(Path fullSizePath, Path medium, Path small,
                      unsigned totalTaskCount, std::function<void()> onFinish,
                      int screenWidth, int screenHeight,
                      std::stop_token stopToken);
  ~ResizeTask() = default;

  void operator()() const;

private:
  Path                  mFullSizePath;
  Path                  mSmallThumbnailOutputPath;
  Path                  mMediumThumbnailOutputPath;
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

  void
  generateThumbnails(Path root, std::vector<Path> mediaMap,
                     std::string                           groupIdentifier,
                     std::function<void(Path, Path, Path)> onThumbnailWritten);

  void abort();
  void abort(Path path);

private:
  std::pair<Path, Path>    assembleOutputPaths(int         index,
                                               std::string groupIdentifier);
  std::shared_ptr<Project> mProject;

  PBDev::ParallelTaskConsumer mParallelTaskConsumer;

  std::function<void(Path, Path, Path)>      mThumbnailWritten;
  int                                        mScreenWidth = 0;
  int                                        mScreenHeight = 0;
  std::unordered_map<Path, std::stop_source> mStopSources;
};
} // namespace PB