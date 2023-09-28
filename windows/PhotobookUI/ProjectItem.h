#pragma once
#include "ProjectItem.h"
#include "ProjectItem.g.h"

namespace winrt::PhotobookUI::implementation {
struct ProjectItem : ProjectItemT<ProjectItem> {

  winrt::hstring name();

private:
  std::string mName;
};
} // namespace winrt::PhotobookUI::implementation
namespace winrt::PhotobookUI::factory_implementation {
struct ProjectItem
    : ProjectItemT<ProjectItem, implementation::ProjectItem> {};
} // namespace winrt::PhotobookUI::factory_implementation
