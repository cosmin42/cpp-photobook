#pragma once

#include "Settings.g.h"
#include "Settings.h"

#include "PaperSettings.g.h"
#include "PaperSettings.h"

#include "ProjectMetadataEntry.h"

#include <pb/services/ProjectManagementService.h>

using namespace PB::Service;

namespace winrt::PhotobookRuntimeComponent::implementation {
struct Settings : SettingsT<Settings> {
  Settings(std::shared_ptr<ProjectManagementService> projectManagementService)
      : mProjectManagementService(projectManagementService)
  {
  }
  ~Settings() = default;

  void RecallProject(winrt::hstring projectId)
  {
    auto               generator = boost::uuids::string_generator();
    boost::uuids::uuid parsedUuid = generator(winrt::to_string(projectId));

    mProjectManagementService->recallMetadata();
  }

  void NewProject(PhotobookRuntimeComponent::PaperSettings paperSettings)
  {
    auto nativePaperSettings =
        winrt::get_self<
            winrt::PhotobookRuntimeComponent::implementation::PaperSettings>(
            paperSettings)
            ->Unwrap();

    mProjectManagementService->newProject(nativePaperSettings);
  }

  void RecallProjectByName(winrt::hstring name)
  {
    mProjectManagementService->loadProject(winrt::to_string(name));
  }

  void RecallMetadata() { mProjectManagementService->recallMetadata(); }

  winrt::hstring ProjectId(winrt::hstring name)
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    auto uuid = maybeLoadedProjectInfo->id;

    return winrt::to_hstring(boost::uuids::to_string(uuid));
  }

  bool HasUUID(winrt::hstring name)
  {
    auto metadata = mProjectManagementService->metadata();
    return metadata.right.find(winrt::to_string(name)) != metadata.right.end();
  }

  void RemoveById(winrt::hstring projectId)
  {
    auto               projectIdStr = winrt::to_string(projectId);
    boost::uuids::uuid parsedUuid =
        boost::uuids::string_generator()(projectIdStr);
    mProjectManagementService->deleteProject(parsedUuid);
  }

  void Clear() { mProjectManagementService->unloadProject(); }

  bool Contains(winrt::hstring name)
  {
    auto metadata = mProjectManagementService->metadata();
    return metadata.right.find(winrt::to_string(name)) != metadata.right.end();
  }

  Windows::Foundation::Collections::IVector<winrt::hstring> ProjectsNames()
  {
    auto projectsNames =
        winrt::single_threaded_observable_vector<winrt::hstring>();

    auto metadata = mProjectManagementService->metadata();
    for (auto it : metadata.right) {
      projectsNames.Append(winrt::to_hstring(it.first));
    }

    return projectsNames;
  }

  Windows::Foundation::Collections::IVector<
      PhotobookRuntimeComponent::ProjectMetadataEntry>
  ProjectsList()
  {
    auto projectsList = winrt::single_threaded_observable_vector<
        PhotobookRuntimeComponent::ProjectMetadataEntry>();

    auto projects = mProjectManagementService->projectsList();

    for (auto &project : projects) {
      auto projectId = boost::uuids::to_string(std::get<0>(project));
      auto entry = winrt::make<ProjectMetadataEntry>(
          projectId, std::get<2>(project).string(), std::get<1>(project));
      projectsList.Append(entry);
    }
    return projectsList;
  }

  void Rename(winrt::hstring newName, winrt::hstring oldName)
  {
    mProjectManagementService->renameProject(winrt::to_string(oldName),
                                             winrt::to_string(newName));
  }

  void Save()
  {
    mProjectManagementService->save();
  }

  bool HasProjectOpen()
  {
    return mProjectManagementService->maybeLoadedProjectInfo() != nullptr;
  }

  winrt::hstring CurrentProjectUUID()
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    auto uuid = maybeLoadedProjectInfo->id;

    return winrt::to_hstring(boost::uuids::to_string(uuid));
  }

  winrt::hstring Hash(winrt::hstring path) { return winrt::hstring(); }

  PhotobookRuntimeComponent::PaperSettings GetPaperSettings()
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    return winrt::make<PaperSettings>(
        maybeLoadedProjectInfo->value.paperSettings);
  }

private:
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
