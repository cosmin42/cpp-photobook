#pragma once

#include "Settings.g.h"
#include "Settings.h"

#include "PaperSettings.g.h"
#include "PaperSettings.h"

#include "ProjectMetadataEntry.h"

#include <pb/ProjectManagementSystem.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct Settings : SettingsT<Settings> {
  Settings(std::shared_ptr<PB::ProjectManagementSystem> projectManagementSystem)
      : mProjectManagementSystem(projectManagementSystem)
  {
  }
  ~Settings() = default;

  void RecallProject(winrt::hstring projectId)
  {
    auto               generator = boost::uuids::string_generator();
    boost::uuids::uuid parsedUuid = generator(winrt::to_string(projectId));

    mProjectManagementSystem->recallMetadata();
  }

  void RecallProjectByName(winrt::hstring name)
  {
    auto metadata = mProjectManagementSystem->metadata();
    auto id = metadata.right.at(winrt::to_string(name));
    mProjectManagementSystem->loadProject(id);
  }

  void RecallMetadata() { mProjectManagementSystem->recallMetadata(); }

  winrt::hstring ProjectId(winrt::hstring name)
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementSystem->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    auto uuid = maybeLoadedProjectInfo->first;

    return winrt::to_hstring(boost::uuids::to_string(uuid));
  }

  bool HasUUID(winrt::hstring name)
  {
    auto metadata = mProjectManagementSystem->metadata();
    return metadata.right.find(winrt::to_string(name)) != metadata.right.end();
  }

  void RemoveById(winrt::hstring projectId)
  {
    mProjectManagementSystem->deleteProject(winrt::to_string(projectId));
  }
  void RemoveByPath(winrt::hstring path)
  {
    auto metadata = mProjectManagementSystem->metadata();

    auto projectId = metadata.right.at(winrt::to_string(path));
    mProjectManagementSystem->deleteProject(boost::uuids::to_string(projectId));
  }

  void Clear() { mProjectManagementSystem->unloadProject(); }

  bool Contains(winrt::hstring name)
  {
    auto metadata = mProjectManagementSystem->metadata();
    return metadata.right.find(winrt::to_string(name)) != metadata.right.end();
  }

  Windows::Foundation::Collections::IVector<winrt::hstring> ProjectsNames()
  {
    auto projectsNames =
        winrt::single_threaded_observable_vector<winrt::hstring>();

    auto metadata = mProjectManagementSystem->metadata();
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

    auto projects = mProjectManagementSystem->projectsList();

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
    mProjectManagementSystem->renameProject(winrt::to_string(oldName),
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
    std::vector<std::vector<std::shared_ptr<PB::VirtualImage>>>
                                                   nativeUnstagedImages;
    std::vector<std::shared_ptr<PB::VirtualImage>> nativesStagedImages;
    std::vector<Path>                              nativeRoots;

    for (int i = 0; i < (int)unstagedImages.Size(); ++i) {
      std::vector<std::shared_ptr<PB::VirtualImage>> nativeUnstagedLine;
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
    mProjectManagementSystem->saveMetadata();

    //mPersistenceService->save(winrt::to_string(thumbnailsDirectoryName),
    //                          nativeUnstagedImages, nativesStagedImages,
    //                          nativeRoots);
  }

  bool HasProjectOpen()
  {
    return mProjectManagementSystem->maybeLoadedProjectInfo() != nullptr;
  }

  winrt::hstring CurrentProjectUUID()
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementSystem->maybeLoadedProjectInfo();
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
    std::vector<std::vector<std::shared_ptr<PB::VirtualImage>>>
                                                   nativeUnstagedImages;
    std::vector<std::shared_ptr<PB::VirtualImage>> nativesStagedImages;
    std::vector<Path>                              nativeRoots;

    for (int i = 0; i < (int)unstagedImages.Size(); ++i) {
      std::vector<std::shared_ptr<PB::VirtualImage>> nativeUnstagedLine;
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

    //return mPersistenceService->isSaved(nativeUnstagedImages,
    //                                    nativesStagedImages, nativeRoots);
    return false;
  }

  winrt::hstring Hash(winrt::hstring path) { return winrt::hstring(); }

  PhotobookRuntimeComponent::PaperSettings GetPaperSettings()
  {
    auto maybeLoadedProjectInfo =
        mProjectManagementSystem->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

    return winrt::make<PaperSettings>(
        maybeLoadedProjectInfo->second.paperSettings);
  }

private:
  std::shared_ptr<PB::ProjectManagementSystem> mProjectManagementSystem =
      nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
