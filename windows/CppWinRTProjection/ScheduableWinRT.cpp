#include <ScheduableWinRT.h>

namespace winrt::CppWinRTProjection::implementation {

void WinRTMainThread::post(std::function<void()> f)
{
  mMainThreadDispatcher.RunAsync(
      winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                                 std::forward<std::function<void()>>(f));
}

} // namespace winrt::CppWinRTProjection::implementation