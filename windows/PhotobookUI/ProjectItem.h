#pragma once
#include "ProjectItem.g.h"
#include "ProjectItem.h"

namespace winrt::PhotobookUI::implementation {
struct ProjectItem : ProjectItemT<ProjectItem> {
  ProjectItem(winrt::hstring itemName);
  winrt::hstring Name();

private:
  winrt::hstring mName;
};
} // namespace winrt::PhotobookUI::implementation
namespace winrt::PhotobookUI::factory_implementation {
struct ProjectItem : ProjectItemT<ProjectItem, implementation::ProjectItem> {};
} // namespace winrt::PhotobookUI::factory_implementation
