#pragma once

#include <filesystem>
#include <set>
#include <thread>
#include <vector>

#include <pb/Config.h>
#include <pb/components/MapReducer.h>
#include <pb/components/RuntimeUUID.h>
#include <pb/image/ImageOperations.h>

namespace PB::Job {
class PicturesSearchJobListener {
public:
  virtual ~PicturesSearchJobListener() = default;
  virtual void onPicturesSearchFinished(PBDev::ThumbnailsJobId jobId, Path root,
                                        std::vector<Path> searchResults) = 0;
  virtual void onPicturesSearchAborted(Path root) = 0;
};

class PicturesSearchJob final : public MapReducer {
public:
  static bool isValidPath(Path path)
  {
    if (std::filesystem::is_regular_file(path) &&
        Process::validExtension(path)) {
      return true;
    }

    if (std::filesystem::is_directory(path)) {
      return true;
    }
    return false;
  }

  PicturesSearchJob(PBDev::ThumbnailsJobId jobId, Path root)
      : mRoot(root), mJobId(jobId)
  {
  }
  ~PicturesSearchJob() {}

  void setPicturesSearchJobListener(PicturesSearchJobListener *listener)
  {
    mListener = listener;
  }

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mCrunchedFlag) {
      return std::nullopt;
    }

    mStopToken = stopToken;

    IdentifyableFunction f{
        RuntimeUUID::newUUID(), [this, stopToken{stopToken}]() {
          auto recursiveIterator =
              std::filesystem::recursive_directory_iterator(
                  mRoot,
                  std::filesystem::directory_options::skip_permission_denied);
          while (recursiveIterator != std::filesystem::end(recursiveIterator) &&
                 !stopToken.stop_requested()) {
            auto path = recursiveIterator->path();
            if (isValidPath(path)) {
              Noir::inst().getLogger()->info("Picture search found {}",
                                             path.string());
              mSearchResults.push_back(path);
            }
            recursiveIterator++;
            if (OneConfig::SIMULATE_SLOW_MAPPER) {
              std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
          }
        }};
    mCrunchedFlag = true;
    return f;
  }

  void onTaskFinished(PBDev::MapReducerTaskId id) override
  {
    UNUSED(id)
    if (mStopToken.stop_requested()) {
      mListener->onPicturesSearchAborted(mRoot);
    }
    else {
      mListener->onPicturesSearchFinished(mJobId, mRoot, mSearchResults);
    }
  }

  std::vector<Path> result() const { return mSearchResults; }

  unsigned taskCount() const override { return 1; }

private:
  PicturesSearchJobListener    *mListener = nullptr;
  Path                          mRoot;
  std::vector<Path>             mSearchResults;
  bool                          mCrunchedFlag = false;
  std::stop_token               mStopToken;
  PBDev::ThumbnailsJobId        mJobId;
};

} // namespace PB