#pragma once

#include <memory>
#include <unordered_map>

#include <boost/bimap/bimap.hpp>

#include <pb/Config.h>
#include <pb/image/Image.h>
#include <pb/util/Util.h>

namespace PB {
class ImageMonitorListener {
public:
  virtual ~ImageMonitorListener() = default;

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
  bool     containsRow(Path path, bool subPath = false) const;
  Path     rowPath(unsigned row) const;

  std::vector<Path> rowList() const;

  std::shared_ptr<VirtualImage> image(unsigned row, unsigned index) const;
  std::shared_ptr<VirtualImage> image(Path full) const;
  std::pair<int, int>           position(Path full) const;

  std::vector<std::vector<std::shared_ptr<VirtualImage>>> const &
  unstaged() const;

  auto statefulIterator(Path root)
      -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>;
  auto statefulIterator(unsigned row)
      -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>;

  void log() const;

private:
  ImageMonitorListener                                   *mListener = nullptr;
  boost::bimaps::bimap<Path, int>                         mRowIndexes;
  boost::bimaps::bimap<Path, std::pair<int, int>>         mPositions;
  std::vector<std::vector<std::shared_ptr<VirtualImage>>> mUnstagedImagesMatrix;
  std::unordered_set<int>                                 mPendingRows;
};

} // namespace PB