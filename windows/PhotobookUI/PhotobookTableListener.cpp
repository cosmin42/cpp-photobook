#include "PhotobookTableListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

PhotobookTableListener::PhotobookTableListener(TableContentPage &parent)
    : mParent(parent)
{
}

void PhotobookTableListener::onMappingStarted(Path path)
{
  mParent.OnMappingStarted(path);
}

void PhotobookTableListener::onMappingFinished(Path path)
{
  mParent.Post([this, path]() { mParent.OnMappingFinished(path); });
}

void PhotobookTableListener::onMappingAborted(Path path)
{
  mParent.Post([this, path]() { mParent.OnMappingAborted(path); });
}

void PhotobookTableListener::onImageUpdated(Path root, int row, int index)
{
  mParent.Post(
      [this, row, index]() { mParent.UpdateUnstagedImage(row, index); });
}

void PhotobookTableListener::onProgressUpdate(int progress, int reference)
{
    mParent.OnProgressUpdate(progress, reference);
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

void PhotobookTableListener::onUnstagedImageAdded(Path rootPath, Path full,
                                                  Path medium,
                                                  Path small, int position)
{
  mParent.Post([this, rootPath, full, medium, small, position]() {
    mParent.OnUnstagedImageAdded(rootPath, full, medium, small,
                                 position);
  });
}

void PhotobookTableListener::onError(PBDev::Error error)
{
  mParent.Post([error{error}, this]() { mParent.OnError(error); });
}

void PhotobookTableListener::post(std::function<void()> f) { mParent.Post(f); }
} // namespace winrt::PhotobookUI::implementation
