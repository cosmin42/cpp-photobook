#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/MapReducer.h>
#include <pb/RuntimeUUID.h>
#include <pb/image/ImageOperations.h>
#include <pb/tasks/SequentialTaskConsumer.h>
#include <pb/util/Util.h>

namespace PB {

class MediaMapper final {
public:
  explicit MediaMapper(std::filesystem::path const &root);

  ~MediaMapper() = default;

  void taskStep();
  int  stepsCount() const;
  bool stoppingCondition() const;

  Path              root() const;
  std::vector<Path> importedDirectories() const;

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  Path                                          mRoot;
  std::vector<Path>                             mSubFiles;
};

class PicturesSearchConfigListener {
public:
  virtual void onPicturesSearchFinished(Path              root,
                                        std::vector<Path> searchResults) = 0;
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
  ~PicturesSearchConfig() = default;

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

    IdentifyableFunction f;
    f.first = RuntimeUUID::newUUID();
    f.second = [this, stopToken{stopToken}]() {
      auto recursiveIterator = std::filesystem::recursive_directory_iterator(
          mRoot, std::filesystem::directory_options::skip_permission_denied);
      while (recursiveIterator != std::filesystem::end(recursiveIterator) &&
             !stopToken.stop_requested()) {
        auto path = recursiveIterator->path();
        if (isValidPath(path)) {
          mSearchResults.push_back(path);
        }
        recursiveIterator++;
      }
    };
    mCrunchedFlag = true;
    return f;
  }

  void onFinished(const boost::uuids::uuid id) override
  {
    UNUSED(id);
    mListener->onPicturesSearchFinished(mRoot, mSearchResults);
  }

  std::vector<Path> result() const { return mSearchResults; }

private:
  PicturesSearchConfigListener *mListener = nullptr;
  Path                          mRoot;
  std::vector<Path>             mSearchResults;
  bool                          mCrunchedFlag = false;
};

} // namespace PB