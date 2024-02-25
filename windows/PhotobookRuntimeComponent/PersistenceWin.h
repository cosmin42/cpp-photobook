#pragma once

#include "PersistenceWin.g.h"


namespace winrt::PhotobookRuntimeComponent::implementation {
struct PersistenceWin : PersistenceWinT<PersistenceWin> {
  PersistenceWin() {}

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PersistenceWin
    : PersistenceWinT<PersistenceWin, implementation::PersistenceWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation
