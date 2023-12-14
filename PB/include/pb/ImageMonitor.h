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
  void setListener(ImageMonitorListener *listener);

  void addRow(Path path, std::vector<std::shared_ptr<VirtualImage>> images);
  void removeRow(int index);
  void removeRow(Path path);

  void clear();

  void completeRow(int index);
  bool isPending(Path path) const;
  bool isPending(int index) const;

  unsigned importListSize() const;
  unsigned rowSize(unsigned row);
  unsigned rowIndex(Path path) const;
  bool     containsRow(Path path) const;
  Path     rowPath(unsigned row) const;

  std::vector<Path> rowList() const;

  std::shared_ptr<VirtualImage> image(unsigned row, unsigned index) const;

  auto statefulIterator(Path root)
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;
  auto statefulIterator(unsigned row)
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;

private:
  ImageMonitorListener                                   *mListener;
  boost::bimaps::bimap<Path, int>                         mRowIndexes;
  boost::bimaps::bimap<Path, std::pair<int, int>>         mPositions;
  std::vector<std::vector<std::shared_ptr<VirtualImage>>> mUnstagedImagesMatrix;
  std::unordered_set<int>                                 mPendingRows;
};

} // namespace PB