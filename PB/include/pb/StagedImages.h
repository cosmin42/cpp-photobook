#pragma once

#include <memory>
#include <vector>

#include <pb/image/Image.h>
#include <pb/util/Util.h>

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

  std::vector<std::shared_ptr<VirtualImage>> const &stagedPhotos() const;
  std::shared_ptr<VirtualImage> picture(unsigned index) const;

  auto statefullIterator()
      -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>;

private:
  StagedImagesListener                      *mListener;
  std::vector<std::shared_ptr<VirtualImage>> mStagedPhotos;
};
} // namespace PB