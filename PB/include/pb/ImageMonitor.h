#pragma once

#include <memory>
#include <unordered_set>

#include <boost/bimap/bimap.hpp>

#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/RowProcessingData.h>
#include <pb/infra/BijectiveUnorderedMap.h>

DECLARE_STRONG_UUID(ImageId)

namespace PB {

class ImageMonitor {
public:
  void addRow(Path importFolderPath,
              std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                 boost::hash<PBDev::ImageId>>
                  images);
  void removeRow(int index);
  void removeRow(Path path);

  std::vector<Path> rowList() const;
  unsigned          rowSize(unsigned row);
  Path              rowPathByIndex(unsigned row) const;
  unsigned          rowIndexByPath(Path path) const;

  void updateImage(PBDev::ImageId imageId, GenericImagePtr image);

  std::pair<PBDev::ImageId, GenericImagePtr> image(unsigned row,
                                                   unsigned index) const;

  void clear();

  std::pair<int, int> position(PBDev::ImageId) const;

  std::vector<std::vector<PBDev::ImageId>> const &unstaged() const;

  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>> const &
  imagesSet() const;

  GenericImagePtrMatrix imagesMatrix() const;

  void log() const;

private:
  boost::bimaps::bimap<Path, int> importedPathsIndices = {};

  infra::BijectiveUnorderedMap<PBDev::ImageId, std::pair<int, int>>
      mPositionsV2;

  std::vector<std::vector<PBDev::ImageId>> mUnstagedImagesMatrix;
  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>>
      mImages;

  FRIEND_TEST(TestImageMonitor, Test0);
};

} // namespace PB