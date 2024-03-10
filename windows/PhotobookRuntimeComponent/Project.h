#pragma once

#include "Project.g.h"
#include "Project.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct Project : ProjectT<Project> {
  Project() {}
  ~Project() = default;

  void f() {}

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
