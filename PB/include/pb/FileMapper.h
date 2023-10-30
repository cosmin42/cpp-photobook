#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/util/CircularIterator.h>
#include <pb/util/Concepts.h>
#include <pb/util/Observable.h>
#include <pb/util/Thread.h>
#include <pb/util/Traits.h>

namespace PB {

class MediaMapper final : public Thread, public ObservableSubject {
public:
  explicit MediaMapper(std::filesystem::path const &root)
      : Thread(Context::inst().sStopSource.get_token()), mRoot(root)
  {
    printDebug("MediaMapper constructor.\n");
    mRecursiveIterator = std::filesystem::recursive_directory_iterator(
        root, std::filesystem::directory_options::skip_permission_denied);
  }

  ~MediaMapper() { printDebug("Destroying MediaMapper\n"); }

  void notify() override
  {
    for (auto observer : mObservers) {
      observer->update(*this);
    }
  }

  void executeSingleTask() override
  {
    if (mRecursiveIterator == std::filesystem::end(mRecursiveIterator)) {
      stop();
    }
    else {
      auto path = mRecursiveIterator->path();
      mImportedFolders.push_back(path);
      mRecursiveIterator++;
    }
    mState = MediaMapState::Started;
    notify();
  }

  void finish() override
  {
    mState = MediaMapState::Finished;
    notify();
  }

  MediaMapState state() const { return mState; }

  Path root() const { return mRoot; }

  std::vector<Path> importedDirectories() const { return mImportedFolders; }

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  Path                                          mRoot;
  std::vector<Path>                             mImportedFolders;
  MediaMapState                                 mState = MediaMapState::None;
};

} // namespace PB