#pragma once

#include "ProjectItem.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ProjectItem : ProjectItemT<ProjectItem> {
  ProjectItem() {}
  ProjectItem(winrt::hstring itemName, winrt::hstring fullPath,
              winrt::hstring name);
  winrt::hstring ItemId();
  winrt::hstring FullPath();
  winrt::hstring Name();

private:
  winrt::hstring mId;
  winrt::hstring mFullPath;
  winrt::hstring mName;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct ProjectItem : ProjectItemT<ProjectItem, implementation::ProjectItem> {};
} // namespace winrt::PhotobookUI::factory_implementation
