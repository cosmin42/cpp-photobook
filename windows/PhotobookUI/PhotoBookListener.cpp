#include "PhotoBookListener.h"
#include "pch.h"

#include <TableContentPage.xaml.h>

namespace winrt::PhotobookUI::implementation {

PhotoBookListener::PhotoBookListener(TableContentPage &parent) : mParent(parent)
{
}
void PhotoBookListener::onFinished()
{
  post([this]() { mParent.onFinished(); });
}
void PhotoBookListener::onStopped()
{
  post([this]() { mParent.onStopped(); });
}
void PhotoBookListener::onStarted()
{
  post([this]() { mParent.onStarted(); });
}
void PhotoBookListener::onPaused()
{
  post([this]() { mParent.onPaused(); });
}
void PhotoBookListener::onResumed()
{
  post([this]() { mParent.onResumed(); });
}

void PhotoBookListener::onProgressUpdate()
{
  post([this]() { mParent.onProgressUpdate(); });
}
void PhotoBookListener::onError(PB::Error error)
{
  post([error{error}, this]() { mParent.onError(error); });
}

void PhotoBookListener::post(std::function<void()> f)
{
  mParent.post(std::forward<std::function<void()>>(f));
}
} // namespace winrt::PhotobookUI::implementation
