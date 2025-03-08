#pragma once

#include <memory>
#include <vector>

#include <pb/entities/GenericImage.h>

namespace PB {

// TODO: Naming in this class i really bad
class StagedImages final {
public:
  void configure(std::vector<GenericImagePtr> &stagedImages);

  void addPictures(std::vector<GenericImagePtr> pictures);
  void addPictures(std::vector<GenericImagePtr> pictures, int position);
  void addPicture(GenericImagePtr picture);

  void removePicture(std::vector<unsigned> indexes);
  void clear();

  void stashImages(std::vector<unsigned> indexes);
  void popImages(int position);

  std::vector<GenericImagePtr> const &stagedPhotos() const;
  GenericImagePtr                     picture(unsigned index) const;

private:
  std::vector<GenericImagePtr> mStagedPhotos;
  std::vector<GenericImagePtr> mStash;
};
} // namespace PB