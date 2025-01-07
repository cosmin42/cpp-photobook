#include <pb/StagedImages.h>

namespace PB {

void StagedImages::configure(std::vector<GenericImagePtr> &stagedImages)
{
  for (int i = 0; i < stagedImages.size(); ++i) {
    mStagedPhotos.push_back(stagedImages.at(i));
  }
}

void StagedImages::addPictures(std::vector<GenericImagePtr> pictures)
{
  mStagedPhotos.insert(mStagedPhotos.end(), pictures.begin(), pictures.end());
}

void StagedImages::addPictures(std::vector<GenericImagePtr> pictures,
                               int                          position)
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
}

void StagedImages::addPicture(GenericImagePtr picture)
{
  mStagedPhotos.push_back(picture);
}

void StagedImages::removePicture(std::vector<unsigned> indexes)
{
  std::sort(indexes.begin(), indexes.end(), std::greater<int>());
  for (int i = 0; i < (int)indexes.size(); ++i) {
    mStagedPhotos.erase(mStagedPhotos.begin() + indexes.at(i));
  }
}

void StagedImages::stashImages(std::vector<unsigned> indexes)
{
  PBDev::basicAssert(mStash.empty());
  std::sort(indexes.begin(), indexes.end(), std::greater<int>());
  for (int i = 0; i < (int)indexes.size(); ++i) {
    mStash.push_back(mStagedPhotos.at(indexes.at(i)));
    mStagedPhotos.erase(mStagedPhotos.begin() + indexes.at(i));
  }
}

void StagedImages::popImages(int position)
{
  addPictures(mStash, position);
  mStash.clear();
}

void StagedImages::clear()
{
  std::vector<unsigned> clearedIndexes;
  for (unsigned i = 0; i < (unsigned)mStagedPhotos.size(); ++i) {
    clearedIndexes.push_back(i);
  }
  mStagedPhotos.clear();
}

std::vector<GenericImagePtr> const &StagedImages::stagedPhotos() const
{
  return mStagedPhotos;
}

GenericImagePtr StagedImages::picture(unsigned index) const
{
  PBDev::basicAssert(index < mStagedPhotos.size());
  return mStagedPhotos.at(index);
}

} // namespace PB