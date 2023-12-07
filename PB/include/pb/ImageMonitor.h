#pragma once

#include <memory>
#include <unordered_map>

#include <boost/bimap/bimap.hpp>

#include <pb/Config.h>
#include <pb/RegularImage.h>
#include <pb/VirtualImage.h>
#include <pb/util/CircularIterator.h>

namespace PB {
class ImageMonitorListener {
public:
  virtual void onImportFolderAdded() = 0;
  virtual void onImportFolderRemoved(unsigned index) = 0;
  virtual void onBatchImportFolderRemoved(std::vector<unsigned> indexes) = 0;
};

class ImageMonitor {
public:
  void setListener(std::shared_ptr<ImageMonitorListener> listener);

  void addGroup(Path path, std::vector<Path> imagesPaths);
  void removeGroup(int index);
  void removeGroup(Path path);

  void clear();

private:
  std::shared_ptr<ImageMonitorListener> mListener;

  boost::bimaps::bimap<Path, int> mGroupIndexes;

  boost::bimaps::bimap<Path, std::pair<int, int>>         mSupportByFullPath;
  std::vector<std::vector<std::shared_ptr<RegularImage>>> mUnstagedImagesMatrix;
};

} // namespace PB