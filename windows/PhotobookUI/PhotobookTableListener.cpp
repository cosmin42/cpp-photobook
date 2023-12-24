#include "PhotobookTableListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

PhotobookTableListener::PhotobookTableListener(TableContentPage &parent)
    : mParent(parent)
{
}
void PhotobookTableListener::onFinished(Path rootPath)
{
  mParent.Post(
      [this, rootPath]() { mParent.OnThumbnailsProcessingFinished(rootPath); });
}

void PhotobookTableListener::onMappingFinished(Path rootPath)
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

void PhotobookTableListener::onProgressUpdate(Path rootPath, int progress,
                                              int reference)
{
  mParent.Post([this, rootPath, progress, reference]() {
    mParent.OnProgressUpdate(rootPath, progress, reference);
  });
}

void PhotobookTableListener::onExportProgressUpdate(int progress, int reference)
{
  mParent.Post([this, progress, reference]() {
    mParent.OnExportProgressUpdate(progress, reference);
  });
}

void PhotobookTableListener::onExportFinished()
{
  mParent.Post([this]() { mParent.OnExportFinished(); });
}

void PhotobookTableListener::onStagedImageAdded(
    std::vector<std::shared_ptr<PB::VirtualImage>> image,
    [[maybe_unused]] int                           index)
{
  mParent.OnStagedImageAdded(image);
}

void PhotobookTableListener::onStagedImageRemoved(
    std::vector<unsigned> removedIndexes)
{
  mParent.OnStagedImageRemoved(removedIndexes);
}

void PhotobookTableListener::onUnstagedImageAdded(Path rootPath,
                                                  Path fullPath,
                                                  Path mediumPath,
                                                  Path smallPath,
                                                  int      position)
{
  mParent.Post([this, rootPath, fullPath, mediumPath, smallPath, position]() {
    mParent.OnUnstagedImageAdded(rootPath, fullPath, mediumPath, smallPath,
                                 position);
  });
}

void PhotobookTableListener::onError(PBDev::Error error)
{
  mParent.Post([error{error}, this]() { mParent.OnError(error); });
}

void PhotobookTableListener::post(std::function<void()> f) { mParent.Post(f); }
} // namespace winrt::PhotobookUI::implementation
