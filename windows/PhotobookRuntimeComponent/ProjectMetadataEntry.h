#pragma once

#include "ProjectMetadataEntry.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {

struct ProjectMetadataEntry : ProjectMetadataEntryT<ProjectMetadataEntry> {

  ProjectMetadataEntry(std::string projectId, std::string projectPath,
                       std::string name)
      : mId(projectId), mProjectPath(projectPath), mName(name)
  {
  }

  winrt::hstring Name() { return winrt::to_hstring(mName); }
  winrt::hstring ProjectId() { return winrt::to_hstring(mId); }
  winrt::hstring ProjectPath() { return winrt::to_hstring(mProjectPath); }

private:
  std::string mName;
  std::string mProjectPath;
  std::string mId;
};

} // namespace winrt::PhotobookRuntimeComponent::implementation
