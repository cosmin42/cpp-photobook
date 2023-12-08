#pragma once

#include <memory>
#include <unordered_map>

#include <boost/bimap/bimap.hpp>

#include <pb/Config.h>
#include <pb/VirtualImage.h>
#include <pb/util/CircularIterator.h>

namespace PB {
class ImageMonitorListener {
public:
  virtual void onImportFolderAdded() = 0;
  virtual void onImportFolderRemoved(unsigned index) = 0;
  virtual void onCleared() = 0;
};

class ImageMonitor {
public:
  void setListener(std::shared_ptr<ImageMonitorListener> listener);

  void addGroup(Path path, std::vector<std::shared_ptr<VirtualImage>> images);
  void removeGroup(int index);
  void removeGroup(Path path);

  void clear();

  void completeGroup(int index);

  unsigned                      importListSize() const;
  unsigned                      rowSize(unsigned row);
  std::shared_ptr<VirtualImage> image(unsigned row, unsigned index) const;

private:
  std::shared_ptr<ImageMonitorListener> mListener;

  boost::bimaps::bimap<Path, int> mGroupIndexes;

  boost::bimaps::bimap<Path, std::pair<int, int>> mPositions;

  std::vector<std::vector<std::shared_ptr<VirtualImage>>> mUnstagedImagesMatrix;

  std::unordered_set<int> mPendingGroups;
};

} // namespace PB