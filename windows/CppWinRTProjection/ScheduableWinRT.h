#pragma once

#include <pb/Scheduable.h>

namespace winrt::CppWinRTProjection::implementation {
class WinRTMainThread final : PB::Scheduable {
public:
  void post(std::function<void()>) override;
};

} // namespace winrt::CppWinRTProjection::implementation