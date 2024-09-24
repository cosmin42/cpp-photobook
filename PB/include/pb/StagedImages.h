#pragma once

#include <memory>
#include <vector>

#include <pb/image/VirtualImage.h>
#include <pb/util/IteratorWithState.h>

namespace PB {

class StagedImages final {
public:
  void configure(std::vector<std::shared_ptr<VirtualImage>> &stagedImages);

  void addPictures(std::vector<std::shared_ptr<VirtualImage>> pictures);
  void addPictures(std::vector<std::shared_ptr<VirtualImage>> pictures,
                   int                                        position);
  void addPicture(std::shared_ptr<VirtualImage> picture);

  void removePicture(std::vector<unsigned> indexes);
  void clear();

  void stashImages(std::vector<unsigned> indexes);
  void popImages(int position);

  std::vector<std::shared_ptr<VirtualImage>> const &stagedPhotos() const;
  std::shared_ptr<VirtualImage> picture(unsigned index) const;

  auto statefullIterator()
      -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>;

private:
  std::vector<std::shared_ptr<VirtualImage>> mStagedPhotos;
  std::vector<std::shared_ptr<VirtualImage>> mStash;
};
} // namespace PB