#include <pb/ImageMonitor.h>

namespace PB {
void ImageMonitor::setListener(std::shared_ptr<ImageMonitorListener> listener)
{
  mListener = listener;
}

void ImageMonitor::addGroup(Path path, std::vector<Path> imagesPaths)
{
  std::vector<std::shared_ptr<RegularImage>> newImages;
  for (int i = 0; i < (int)imagesPaths.size(); ++i) {
    newImages.push_back(std::make_shared<RegularImage>());
  }

  mUnstagedImagesMatrix.push_back(newImages);

  mGroupIndexes.insert({path, (int)mUnstagedImagesMatrix.size() - 1});
}

void ImageMonitor::removeGroup(int index) { mGroupIndexes.right.erase(index); }

void ImageMonitor::removeGroup(Path path) { mGroupIndexes.left.erase(path); }

void ImageMonitor::clear() {}

} // namespace PB