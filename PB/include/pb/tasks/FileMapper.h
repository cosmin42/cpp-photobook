#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/Config.h>
#include <pb/MapReducer.h>
#include <pb/RuntimeUUID.h>
#include <pb/image/ImageOperations.h>

// TODO: Rename FileMapper
namespace PB {
class PicturesSearchConfigListener {
public:
  virtual ~PicturesSearchConfigListener() = default;
  virtual void onPicturesSearchFinished(Path              root,
                                        std::vector<Path> searchResults) = 0;
  virtual void onPicturesSearchAborted(Path root) = 0;
};

class PicturesSearchConfig final : public MapReducer {
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

  PicturesSearchConfig(Path root) : mRoot(root) {}
  ~PicturesSearchConfig() {}

  void setPicturesSearchConfigListener(PicturesSearchConfigListener *listener)
  {
    mListener = listener;
  }

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override
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

  void onFinished(PBDev::MapReducerTaskId id) override
  {
    UNUSED(id)
    if (mStopToken.stop_requested()) {
      mListener->onPicturesSearchAborted(mRoot);
    }
    else {
      mListener->onPicturesSearchFinished(mRoot, mSearchResults);
    }
  }

  std::vector<Path> result() const { return mSearchResults; }

private:
  PicturesSearchConfigListener *mListener = nullptr;
  Path                          mRoot;
  std::vector<Path>             mSearchResults;
  bool                          mCrunchedFlag = false;
  std::stop_token               mStopToken;
};

} // namespace PB