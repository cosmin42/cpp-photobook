#include <pb/StagedImages.h>

namespace PB {

void StagedImages::setListener(std::shared_ptr<StagedImagesListener> listener)
{
  mListener = listener;
}

void StagedImages::addPicture(
    std::vector<std::shared_ptr<VirtualImage>> pictures)
{
  int nextIndex = (int)mStagedPhotos.size();
  mStagedPhotos.insert(mStagedPhotos.end(), pictures.begin(), pictures.end());

  PB::basicAssert(mListener != nullptr);
  mListener->onPicturesAdded(nextIndex, pictures.size());
}

void StagedImages::addPicture(
    std::vector<std::shared_ptr<VirtualImage>> pictures, int position)
{
  PB::basicAssert(position > -1);
  PB::basicAssert(position <= (int)mStagedPhotos.size());

  if (position < (int)mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.begin() + position, pictures.begin(),
                         pictures.end());
  }
  else {
    mStagedPhotos.insert(mStagedPhotos.end(), pictures.begin(), pictures.end());
  }

  PB::basicAssert(mListener != nullptr);
  mListener->onPicturesAdded(position, (int)pictures.size());
}

void StagedImages::removePicture(std::vector<unsigned> indexes)
{
  std::sort(indexes.begin(), indexes.end(), std::greater<int>());
  for (int i = 0; i < (int)indexes.size(); ++i) {
    mStagedPhotos.erase(mStagedPhotos.begin() + indexes.at(i));
  }
  mListener->onPictureRemoved(indexes);
}

void StagedImages::clear()
{
  std::vector<unsigned> clearedIndexes;
  for (unsigned i = 0; i < (unsigned)mStagedPhotos.size(); ++i) {
    clearedIndexes.push_back(i);
  }
  mStagedPhotos.clear();
  mListener->onPictureRemoved(clearedIndexes);
}

std::vector<std::shared_ptr<VirtualImage>> const &
StagedImages::stagedPhotos() const
{
  return mStagedPhotos;
}

std::shared_ptr<VirtualImage> StagedImages::picture(unsigned index) const
{
  PB::basicAssert(index < mStagedPhotos.size());
  return mStagedPhotos.at(index);
}

} // namespace PB