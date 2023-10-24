#include "PhotobookTableListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

PhotobookTableListener::PhotobookTableListener(TableContentPage &parent)
    : mParent(parent)
{
}
void PhotobookTableListener::onFinished(PB::Path rootPath)
{
  mParent.Post(
      [this, rootPath]() { mParent.OnThumbnailsProcessingFinished(rootPath); });
}

void PhotobookTableListener::onMappingFinished(PB::Path rootPath)
{
  mParent.Post([this, rootPath]() { mParent.OnMappingFinished(rootPath); });
}

void PhotobookTableListener::onStopped()
{
  mParent.Post([this]() { mParent.OnMappingStopped(); });
}
void PhotobookTableListener::onStarted()
{
  mParent.Post([this]() { mParent.OnMappingStarted(); });
}
void PhotobookTableListener::onPaused()
{
  mParent.Post([this]() { mParent.OnMappingPaused(); });
}
void PhotobookTableListener::onResumed()
{
  mParent.Post([this]() { mParent.OnMappingResumed(); });
}

void PhotobookTableListener::onProgressUpdate(PB::Path rootPath, int progress,
                                              int reference)
{
  mParent.Post([this, rootPath, progress, reference]() {
    mParent.OnProgressUpdate(rootPath, progress, reference);
  });
}

void PhotobookTableListener::onStagedImageAdded(PB::Thumbnails       image,
                                           [[maybe_unused]] int index)
{
  mParent.OnStagedImageAdded(image);
}

void PhotobookTableListener::onStagedImageRemoved(
    std::vector<int> removedIndexes)
{
  mParent.OnStagedImageRemoved(removedIndexes);
}

void PhotobookTableListener::onUnstagedImageAdded(PB::Path rootPath,
                                             PB::Path fullPath,
                                             PB::Path mediumPath,
                                             PB::Path smallPath, int position)
{
  mParent.Post([this, rootPath, fullPath, mediumPath, smallPath, position]() {
    mParent.OnUnstagedImageAdded(rootPath, fullPath, mediumPath, smallPath,
                                 position);
  });
}

void PhotobookTableListener::onAddingUnstagedImagePlaceholder(unsigned size)
{
  mParent.Post(
      [this, size]() { mParent.OnAddingUnstagedImagePlaceholder(size); });
}

void PhotobookTableListener::onError(PB::Error error)
{
  mParent.Post([error{error}, this]() { mParent.OnError(error); });
}

void PhotobookTableListener::post(std::function<void()> f) { mParent.Post(f); }
} // namespace winrt::PhotobookUI::implementation
