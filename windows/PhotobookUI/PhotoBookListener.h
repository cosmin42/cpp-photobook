#pragma once

#include <functional>

#include <pb/Error.h>

namespace winrt::PhotobookUI::implementation {
struct TableContentPage;

class PhotoBookListener {
public:
  explicit PhotoBookListener(TableContentPage &parent);
  void onFinished();
  void onStopped();
  void onStarted();
  void onPaused();
  void onResumed();

  void onProgressUpdate();
  void onError(PB::Error error);

  void post(std::function<void()> f);

private:
  TableContentPage &mParent;
};
} // namespace winrt::PhotobookUI::implementation