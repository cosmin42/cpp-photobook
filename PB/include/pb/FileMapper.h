#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/MediaMapListener.h>
#include <pb/util/CircularIterator.h>
#include <pb/util/Concepts.h>
#include <pb/util/Thread.h>
#include <pb/util/Traits.h>

namespace PB {

class MediaMapper final : public Thread {
public:
  explicit MediaMapper(
      std::filesystem::path const                     &root,
      std::shared_ptr<MediaMapListener> listener)
      : Thread(Context::inst().sStopSource.get_token()), mListener(listener),
        mRoot(root)
  {
    printDebug("MediaMapper constructor.\n");
    mRecursiveIterator = std::filesystem::recursive_directory_iterator(
        root, std::filesystem::directory_options::skip_permission_denied);
  }

  MediaMapper &operator=(MediaMapper const &other) = delete;

  ~MediaMapper() { printDebug("Destroying MediaMapper\n"); }

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
    mListener->onProgressUpdate();
  }

  void finish() override { mListener->onFinished(mImportedFolders, mRoot); }

private:
  std::shared_ptr<MediaMapListener> mListener;
  std::filesystem::recursive_directory_iterator    mRecursiveIterator;
  Path                                             mRoot;
  std::vector<Path>                                mImportedFolders;
};

} // namespace PB