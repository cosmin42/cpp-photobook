#include "PhotoBookListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

PhotoBookListener::PhotoBookListener(TableContentPage &parent) : mParent(parent)
{
}
void PhotoBookListener::onFinished()
{
  mParent.Post([this]() { mParent.OnThumbnailsProcessingFinished(); });
}

void PhotoBookListener::onMappingFinished()
{
  mParent.Post([this]() { mParent.OnMappingFinished(); });
}

void PhotoBookListener::onStopped()
{
  mParent.Post([this]() { mParent.OnMappingStopped(); });
}
void PhotoBookListener::onStarted()
{
  mParent.Post([this]() { mParent.OnMappingStarted(); });
}
void PhotoBookListener::onPaused()
{
  mParent.Post([this]() { mParent.OnMappingPaused(); });
}
void PhotoBookListener::onResumed()
{
  mParent.Post([this]() { mParent.OnMappingResumed(); });
}

void PhotoBookListener::onProgressUpdate(int progress, int reference)
{
  mParent.Post([this, progress, reference]() {
    mParent.OnProgressUpdate(progress, reference);
  });
}

void PhotoBookListener::onStagedImageAdded(PB::Thumbnails       image,
                                           [[maybe_unused]] int index)
{
  mParent.OnStagedImageAdded(image);
}

void PhotoBookListener::onStagedImageRemoved(std::vector<int> removedIndexes)
{
  mParent.OnStagedImageRemoved(removedIndexes);
}

void PhotoBookListener::onUnstagedImageAdded(PB::Path fullPath,
                                             PB::Path mediumPath,
                                             PB::Path smallPath, int position)
{
  mParent.Post([this, fullPath, mediumPath, smallPath, position]() {
    mParent.OnUnstagedImageAdded(fullPath, mediumPath, smallPath, position);
  });
}

void PhotoBookListener::onAddingUnstagedImagePlaceholder(unsigned size)
{
  mParent.Post(
      [this, size]() { mParent.OnAddingUnstagedImagePlaceholder(size); });
}

void PhotoBookListener::onError(PB::Error error)
{
  mParent.Post([error{error}, this]() { mParent.OnError(error); });
}

void PhotoBookListener::post(std::function<void()> f) { mParent.Post(f); }
} // namespace winrt::PhotobookUI::implementation
