#include "PhotobookTableListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

void PhotobookTableListener::onMappingStarted(Path path)
{
  mParent->OnMappingStarted(path);
}

void PhotobookTableListener::onMappingFinished(Path path)
{
  mParent->Post([this, path]() { mParent->OnMappingFinished(path); });
}

void PhotobookTableListener::onMappingAborted(Path path)
{
  mParent->Post([this, path]() { mParent->OnMappingAborted(path); });
}

void PhotobookTableListener::onImageUpdated(Path root, int row, int index)
{
  mParent->Post(
      [this, row, index]() { mParent->UpdateUnstagedImage(row, index); });
}

void PhotobookTableListener::configure(TableContentPage *parent)
{
  mParent = parent;
}

void PhotobookTableListener::onProjectRead() {}

void PhotobookTableListener::onProjectRenamed() { mParent->OnProjectRenamed(); }

void PhotobookTableListener::onMetadataUpdated() {}

void PhotobookTableListener::onPersistenceError(PBDev::Error) {}

void PhotobookTableListener::onExportFinished()
{
  mParent->Post([this]() { mParent->OnExportFinished(); });
}

void PhotobookTableListener::onStagedImageAdded(
    std::vector<std::shared_ptr<PB::VirtualImage>> image,
    [[maybe_unused]] int                           index)
{
  mParent->OnStagedImageAdded(image);
}

void PhotobookTableListener::onStagedImageRemoved(
    std::vector<unsigned> removedIndexes)
{
  mParent->OnStagedImageRemoved(removedIndexes);
}

void PhotobookTableListener::onError(PBDev::Error error)
{
  mParent->Post([error{error}, this]() { mParent->OnError(error); });
}

void PhotobookTableListener::post(std::function<void()> f) { mParent->Post(f); }

void PhotobookTableListener::onProgressUpdate(
    PB::ProgressInfo definedProgress, PB::ProgressInfo undefinedProgress)
{
  mParent->onProgressUpdate(definedProgress, undefinedProgress);
}
} // namespace winrt::PhotobookUI::implementation
