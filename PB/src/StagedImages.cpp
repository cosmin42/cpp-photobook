#include <pb/StagedImages.h>

namespace PB {

void StagedImages::setListener(StagedImagesListener *listener)
{
  mListener = listener;
}

void StagedImages::configure(
    std::vector<std::shared_ptr<VirtualImage>> &stagedImages)
{
  for (int i = 0; i < stagedImages.size(); ++i) {
    mStagedPhotos.push_back(stagedImages.at(i));
  }
}

void StagedImages::addPictures(
    std::vector<std::shared_ptr<VirtualImage>> pictures)
{
  int nextIndex = (int)mStagedPhotos.size();
  mStagedPhotos.insert(mStagedPhotos.end(), pictures.begin(), pictures.end());

  PBDev::basicAssert(mListener != nullptr);
  if (pictures.size() > 0) {
    mListener->onPicturesAdded(nextIndex, (int)pictures.size());
  }
}

void StagedImages::addPictures(
    std::vector<std::shared_ptr<VirtualImage>> pictures, int position)
{
  PBDev::basicAssert(position > -1);
  PBDev::basicAssert(position <= (int)mStagedPhotos.size());

  if (position < (int)mStagedPhotos.size()) {
    mStagedPhotos.insert(mStagedPhotos.begin() + position, pictures.begin(),
                         pictures.end());
  }
  else {
    mStagedPhotos.insert(mStagedPhotos.end(), pictures.begin(), pictures.end());
  }

  PBDev::basicAssert(mListener != nullptr);
  mListener->onPicturesAdded(position, (int)pictures.size());
}

void StagedImages::addPicture(std::shared_ptr<VirtualImage> picture)
{
  mStagedPhotos.push_back(picture);
  PBDev::basicAssert(mListener != nullptr);
  mListener->onPicturesAdded((int)mStagedPhotos.size() - 1,
                             (int)mStagedPhotos.size());
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
  PBDev::basicAssert(index < mStagedPhotos.size());
  return mStagedPhotos.at(index);
}

auto StagedImages::statefullIterator()
    -> PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>
{
  return PBDev::IteratorWithState<std::vector<std::shared_ptr<VirtualImage>>>(
      mStagedPhotos);
}

} // namespace PB