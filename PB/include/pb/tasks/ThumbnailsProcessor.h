#pragma once

#include <pb/RowProcessingData.h>
#include <pb/project/Project.h>
#include <pb/tasks/ParallelTaskConsumer.h>

namespace PB {

class ThumbnailsProcessor final {
public:
  explicit ThumbnailsProcessor(std::shared_ptr<PlatformInfo> platformInfo);
  ~ThumbnailsProcessor();

  void setScreenSize(std::pair<int, int> size);

  void generateThumbnails(
      std::string projectName, Path root, std::vector<ProcessingData> mediaMap,
      std::string                                 groupIdentifier,
      std::function<void(Path, ImageResources)> onThumbnailWritten);

  void abort();
  void abort(Path path);

  void clearJob(Path path);
  void clearJobs();

  static std::pair<Path, Path> assembleOutputPaths(Path        localStatePath,
                                                   int         index,
                                                   std::string groupIdentifier,
                                                   std::string projectName);

private:
  std::pair<Path, Path>    assembleOutputPaths(int         index,
                                               std::string groupIdentifier,
                                               std::string projectName);

  PBDev::ParallelTaskConsumer mParallelTaskConsumer;

  std::function<void(Path, ImageResources)>   mThumbnailWritten;
  int                                         mScreenWidth = 0;
  int                                         mScreenHeight = 0;
  std::unordered_map<Path, std::stop_source>  mStopSources;
  std::shared_ptr<PlatformInfo>               mPlatformInfo = nullptr;
};
} // namespace PB