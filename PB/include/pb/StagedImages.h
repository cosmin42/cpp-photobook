#pragma once

#include <memory>
#include <vector>

#include <pb/image/VirtualImage.h>
#include <pb/util/IteratorWithState.h>

namespace PB {

class StagedImagesListener {
public:
  virtual ~StagedImagesListener() = default;
  virtual void onPicturesAdded(int index, int size) = 0;
  virtual void onPictureRemoved(std::vector<unsigned> index) = 0;
};

class StagedImages final {
public:
  void setListener(StagedImagesListener *listener);

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
  StagedImagesListener                      *mListener = nullptr;
  std::vector<std::shared_ptr<VirtualImage>> mStagedPhotos;
  std::vector<std::shared_ptr<VirtualImage>> mStash;
};
} // namespace PB