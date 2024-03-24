#pragma once

#include "ProjectMetadataEntry.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

struct ProjectMetadataEntry : ProjectMetadataEntryT<ProjectMetadataEntry> {

  ProjectMetadataEntry(std::string name, std::string projectId)
      : mName(name), mId(projectId)
  {
  }

  winrt::hstring Name() { return winrt::to_hstring(mName); }
  winrt::hstring ProjectId() { return winrt::to_hstring(mId); }

private:
  std::string mName;
  std::string mId;
};

} // namespace winrt::PhotobookRuntimeComponent::implementation
