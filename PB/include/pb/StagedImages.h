#pragma once

#include <memory>
#include <vector>

#include <pb/VirtualImage.h>

namespace PB {

class StagedImagesListener {
public:
  virtual void onPicturesAdded(int index, int size) = 0;
  virtual void onPictureRemoved(std::vector<unsigned> index) = 0;
};

class StagedImages final {
public:
  void setListener(std::shared_ptr<StagedImagesListener> listener);

  void addPictures(std::vector<std::shared_ptr<VirtualImage>> pictures);
  void addPictures(std::vector<std::shared_ptr<VirtualImage>> pictures,
                  int                                        position);
  void removePicture(std::vector<unsigned> indexes);
  void clear();

  std::vector<std::shared_ptr<VirtualImage>> const &stagedPhotos() const;
  std::shared_ptr<VirtualImage> picture(unsigned index) const;

private:
  std::shared_ptr<StagedImagesListener>      mListener;
  std::vector<std::shared_ptr<VirtualImage>> mStagedPhotos;
};
} // namespace PB