#pragma once
#include "ProjectItem.g.h"
#include "ProjectItem.h"

namespace winrt::PhotobookUI::implementation {
struct ProjectItem : ProjectItemT<ProjectItem> {
  winrt::hstring name();

  void setName(std::string const &name) { mName = name; }

private:
  std::string mName;
};
} // namespace winrt::PhotobookUI::implementation
namespace winrt::PhotobookUI::factory_implementation {
struct ProjectItem : ProjectItemT<ProjectItem, implementation::ProjectItem> {};
} // namespace winrt::PhotobookUI::factory_implementation
