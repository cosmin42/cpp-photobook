#pragma once

#include "Settings.g.h"
#include "Settings.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct Settings : SettingsT<Settings> {
  Settings() {}
  ~Settings() = default;

  /*
std::shared_ptr<Project> currentProject();
*/
  void recallProject(winrt::hstring projectId) {}

  void recallProjectByName(winrt::hstring name) {}

  void recallMetadata() {}

  winrt::hstring projectId(winrt::hstring name) { return winrt::hstring(); }

  bool hasUUID(winrt::hstring name) { return false; }

  Windows::Foundation::Collections::IVector<winrt::hstring> projectsNames() {}

  // void newProject(String name, std::shared_ptr<Project> project);

  void removeById(winrt::hstring projectId) {}
  void removeByPath(winrt::hstring path) {}

  void clear() {}

  bool contains(winrt::hstring name) {}

  Windows::Foundation::Collections::IVector<ProjectMetadataEntry> ProjectsList()
  {
    auto projectsList =
        winrt::single_threaded_observable_vector<ProjectMetadataEntry>();
    return projectsList;
  }

  void rename(winrt::hstring newName, winrt::hstring oldName) {}

  void save(winrt::hstring thumbnailsDirectoryName,
            Windows::Foundation::Collections::IVector<
                Windows::Foundation::Collections::IVector<
                    PhotobookRuntimeComponent::VirtualImagePtr>>
                unstagedImages,
            Windows::Foundation::Collections::IVector<
                PhotobookRuntimeComponent::VirtualImagePtr>
                stagedImages,
            Windows::Foundation::Collections::IVector<winrt::hstring> root)
  {
  }

  bool           hasProjectOpen() { return false; }
  winrt::hstring currentProjectUUID() { return winrt::hstring(); }

  bool isSaved(Windows::Foundation::Collections::IVector<
                   Windows::Foundation::Collections::IVector<
                       PhotobookRuntimeComponent::VirtualImagePtr>>
                   unstagedImages,
               Windows::Foundation::Collections::IVector<
                   PhotobookRuntimeComponent::VirtualImagePtr>
                   stagedImages,
               Windows::Foundation::Collections::IVector<winrt::hstring> roots)
  {
    return false;
  }
  winrt::hstring hash(winrt::hstring path) { return winrt::hstring(); }

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
