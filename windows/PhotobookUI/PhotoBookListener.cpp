#include "PhotoBookListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

PhotoBookListener::PhotoBookListener(TableContentPage &parent) : mParent(parent)
{
}
void PhotoBookListener::onFinished()
{
  mParent.post([this]() { mParent.onFinished(); });
}
void PhotoBookListener::onStopped()
{
  mParent.post([this]() { mParent.onStopped(); });
}
void PhotoBookListener::onStarted()
{
  mParent.post([this]() { mParent.onStarted(); });
}
void PhotoBookListener::onPaused()
{
  mParent.post([this]() { mParent.onPaused(); });
}
void PhotoBookListener::onResumed()
{
  mParent.post([this]() { mParent.onResumed(); });
}

void PhotoBookListener::onProgressUpdate(int progress, int reference)
{
  mParent.post([this, progress, reference]() {
    mParent.onProgressUpdate(progress, reference);
  });
}

void PhotoBookListener::onUnstagedImageAdded(PB::Path path)
{
  mParent.post([this, path]() { mParent.onUnstagedImageAdded(path); });
}

void PhotoBookListener::onError(PB::Error error)
{
  mParent.post([error{error}, this]() { mParent.onError(error); });
}

void PhotoBookListener::post(std::function<void()> f) { mParent.post(f); }
} // namespace winrt::PhotobookUI::implementation
