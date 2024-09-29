#pragma once

#include "Settings.g.h"
#include "Settings.h"

#include "PaperSettings.g.h"
#include "PaperSettings.h"

#include "ProjectMetadataEntry.h"

#include <pb/services/ProjectManagementService.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct Settings : SettingsT<Settings> {
  Settings(
      std::shared_ptr<PB::ProjectManagementService> projectManagementService)
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

  void RecallProjectByName(winrt::hstring name)
  {
    auto metadata = mProjectManagementService->metadata();
    auto id = metadata.right.at(winrt::to_string(name));
    mProjectManagementService->loadProject(id);
  }

  void RecallMetadata() { mProjectManagementService->recallMetadata(); }

  winrt::hstring ProjectId(winrt::hstring name)
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    auto uuid = maybeLoadedProjectInfo->first;

    return winrt::to_hstring(boost::uuids::to_string(uuid));
  }

  bool HasUUID(winrt::hstring name)
  {
    auto metadata = mProjectManagementService->metadata();
    return metadata.right.find(winrt::to_string(name)) != metadata.right.end();
  }

  void RemoveById(winrt::hstring projectId)
  {
    mProjectManagementService->deleteProject(winrt::to_string(projectId));
  }
  void RemoveByPath(winrt::hstring path)
  {
    auto metadata = mProjectManagementService->metadata();

    auto projectId = metadata.right.at(winrt::to_string(path));
    mProjectManagementService->deleteProject(
        boost::uuids::to_string(projectId));
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

  void Save(winrt::hstring thumbnailsDirectoryName,
            Windows::Foundation::Collections::IVector<
                Windows::Foundation::Collections::IVector<
                    PhotobookRuntimeComponent::VirtualImagePtr>>
                unstagedImages,
            Windows::Foundation::Collections::IVector<
                PhotobookRuntimeComponent::VirtualImagePtr>
                stagedImages,
            Windows::Foundation::Collections::IVector<winrt::hstring> root)
  {
    std::vector<std::vector<PB::GenericImagePtr>> nativeUnstagedImages;
    std::vector<PB::GenericImagePtr>              nativesStagedImages;
    std::vector<Path>                             nativeRoots;

    for (int i = 0; i < (int)unstagedImages.Size(); ++i) {
      std::vector<PB::GenericImagePtr> nativeUnstagedLine;
      for (int j = 0; j < (int)unstagedImages.GetAt(i).Size(); ++j) {
        auto nativeImagePtr =
            winrt::get_self<winrt::PhotobookRuntimeComponent::implementation::
                                VirtualImagePtr>(
                unstagedImages.GetAt(i).GetAt(j))
                ->Unwrap();
        nativeUnstagedLine.push_back(nativeImagePtr);
      }
      nativeUnstagedImages.push_back(nativeUnstagedLine);
    }

    for (int i = 0; i < (int)stagedImages.Size(); ++i) {
      auto nativeImagePtr =
          winrt::get_self<winrt::PhotobookRuntimeComponent::implementation::
                              VirtualImagePtr>(stagedImages.GetAt(i))
              ->Unwrap();
      nativesStagedImages.push_back(nativeImagePtr);
    }

    for (int i = 0; i < (int)root.Size(); ++i) {
      nativeRoots.push_back(winrt::to_string(root.GetAt(i)));
    }
    mProjectManagementService->saveMetadata();

    // mPersistenceService->save(winrt::to_string(thumbnailsDirectoryName),
    //                           nativeUnstagedImages, nativesStagedImages,
    //                           nativeRoots);
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

    auto uuid = maybeLoadedProjectInfo->first;

    return winrt::to_hstring(boost::uuids::to_string(uuid));
  }

  bool IsSaved(Windows::Foundation::Collections::IVector<
                   Windows::Foundation::Collections::IVector<
                       PhotobookRuntimeComponent::VirtualImagePtr>>
                   unstagedImages,
               Windows::Foundation::Collections::IVector<
                   PhotobookRuntimeComponent::VirtualImagePtr>
                   stagedImages,
               Windows::Foundation::Collections::IVector<winrt::hstring> roots)
  {
    std::vector<std::vector<PB::GenericImagePtr>> nativeUnstagedImages;
    std::vector<PB::GenericImagePtr>              nativesStagedImages;
    std::vector<Path>                             nativeRoots;

    for (int i = 0; i < (int)unstagedImages.Size(); ++i) {
      std::vector<PB::GenericImagePtr> nativeUnstagedLine;
      for (int j = 0; j < (int)unstagedImages.GetAt(i).Size(); ++j) {
        auto nativeImagePtr =
            winrt::get_self<winrt::PhotobookRuntimeComponent::implementation::
                                VirtualImagePtr>(
                unstagedImages.GetAt(i).GetAt(j))
                ->Unwrap();
        nativeUnstagedLine.push_back(nativeImagePtr);
      }
      nativeUnstagedImages.push_back(nativeUnstagedLine);
    }

    for (int i = 0; i < (int)stagedImages.Size(); ++i) {
      auto nativeImagePtr =
          winrt::get_self<winrt::PhotobookRuntimeComponent::implementation::
                              VirtualImagePtr>(stagedImages.GetAt(i))
              ->Unwrap();
      nativesStagedImages.push_back(nativeImagePtr);
    }

    for (int i = 0; i < (int)roots.Size(); ++i) {
      nativeRoots.push_back(winrt::to_string(roots.GetAt(i)));
    }

    throw std::runtime_error("Not implemented yet");
    // return mPersistenceService->isSaved(nativeUnstagedImages,
    //                                     nativesStagedImages, nativeRoots);
    return false;
  }

  winrt::hstring Hash(winrt::hstring path) { return winrt::hstring(); }

  PhotobookRuntimeComponent::PaperSettings GetPaperSettings()
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementService->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    return winrt::make<PaperSettings>(
        maybeLoadedProjectInfo->second.paperSettings);
  }

private:
  std::shared_ptr<PB::ProjectManagementService> mProjectManagementService =
      nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
