#pragma once

#include <pb/Scheduable.h>

#include <winrt/Windows.UI.Core.h>

namespace winrt::CppWinRTProjection::implementation {
class WinRTMainThread final : PB::Scheduable {
public:
  void post(std::function<void()>) override;

private:
  winrt::Windows::UI::Core::CoreDispatcher mMainThreadDispatcher;
};

} // namespace winrt::CppWinRTProjection::implementation