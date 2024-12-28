#pragma once

#include <memory>
#include <unordered_set>

#include <boost/bimap/bimap.hpp>

#include <pb/Config.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/RowProcessingData.h>
#include <pb/util/IteratorWithState.h>

namespace PB {

class ImageMonitor {
public:

  void addRow(Path path, std::vector<GenericImagePtr> images);
  void replaceImage(Path path, GenericImagePtr image, int index = -1);
  void removeRow(int index);
  void removeRow(Path path);

  void clear();

  void completeRow(int index);
  void completeRowByPath(Path path);
  bool isPending(Path path) const;
  bool isPending(int index) const;

  unsigned importListSize() const;
  unsigned rowSize(unsigned row);
  unsigned rowIndex(Path path) const;
  bool     containsRow(Path path, bool subPath = false) const;
  Path     rowPath(unsigned row) const;

  std::vector<Path> rowList() const;

  GenericImagePtr     image(unsigned row, unsigned index) const;
  GenericImagePtr     image(Path full) const;
  std::pair<int, int> position(Path full) const;

  std::vector<std::vector<GenericImagePtr>> const &unstaged() const;

  auto statefulIterator(Path root)
      -> PBDev::IteratorWithState<std::vector<GenericImagePtr>>;
  auto statefulIterator(unsigned row)
      -> PBDev::IteratorWithState<std::vector<GenericImagePtr>>;

  void log() const;

private:
  boost::bimaps::bimap<Path, int>                 mRowIndexes = {};
  boost::bimaps::bimap<Path, std::pair<int, int>> mPositions = {};
  std::vector<std::vector<GenericImagePtr>>       mUnstagedImagesMatrix;
  std::unordered_set<int>                         mPendingRows;
};

} // namespace PB