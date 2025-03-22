#pragma once

#include <memory>
#include <unordered_set>

#include <boost/bimap/bimap.hpp>

#include <pb/Config.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/RowProcessingData.h>

DECLARE_STRONG_UUID(ImageId)

namespace PB {

class ImageMonitor {
public:
  void addRow(Path path, std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                            boost::hash<PBDev::ImageId>>
                             images);
  void updateImage(PBDev::ImageId imageId, GenericImagePtr image);

  void removeRow(int index);
  void removeRow(Path path);

  void clear();

  unsigned importsCount() const;

  unsigned rowSize(unsigned row);
  unsigned rowIndex(Path path) const;
  bool     containsRow(Path path, bool subPath = false) const;
  Path     rowPath(unsigned row) const;

  std::vector<Path> rowList() const;

  std::pair<PBDev::ImageId, GenericImagePtr> image(unsigned row,
                                                   unsigned index) const;

  std::pair<int, int> position(PBDev::ImageId) const;

  std::vector<std::vector<PBDev::ImageId>> const &unstaged() const;

  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>> const &
  imagesSet() const;

  GenericImagePtrMatrix imagesMatrix() const;

  void log() const;

private:
  boost::bimaps::bimap<Path, int> importedPathsIndices = {};

  // TODO: Implement a wrapper over the maps to replace bimap
  std::unordered_map<std::pair<int, int>, PBDev::ImageId,
                     boost::hash<std::pair<int, int>>>
      mPositionsV2Reverse;
  std::unordered_map<PBDev::ImageId, std::pair<int, int>,
                     boost::hash<PBDev::ImageId>>
      mPositions;

  std::vector<std::vector<PBDev::ImageId>> mUnstagedImagesMatrix;
  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>>
      mImages;
};

} // namespace PB