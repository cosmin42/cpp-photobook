#pragma once

#include "Settings.g.h"
#include "Settings.h"

#include "PaperSettings.g.h"
#include "PaperSettings.h"

#include "ProjectMetadataEntry.h"

#include <pb/persistence/PersistenceService.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct Settings : SettingsT<Settings> {
  Settings(std::shared_ptr<PB::PersistenceService> persistenceService)
      : mPersistenceService(persistenceService)
  {
  }
  ~Settings() = default;

  void RecallProject(winrt::hstring projectId)
  {
    auto               generator = boost::uuids::string_generator();
    boost::uuids::uuid parsedUuid = generator(winrt::to_string(projectId));

    mPersistenceService->recallProject(parsedUuid);
  }

  void RecallProjectByName(winrt::hstring name)
  {
    mPersistenceService->recallProject(winrt::to_string(name));
  }

  void RecallMetadata() { mPersistenceService->recallMetadata(); }

  winrt::hstring ProjectId(winrt::hstring name)
  {
    auto uuid = mPersistenceService->currentProjectUUID();

    return winrt::to_hstring(boost::uuids::to_string(uuid));
  }

  bool HasUUID(winrt::hstring name)
  {
    return mPersistenceService->hasUUID(winrt::to_string(name));
  }

  // void newProject(String name, std::shared_ptr<Project> project);

  void RemoveById(winrt::hstring projectId)
  {
    auto generator = boost::uuids::string_generator();
    auto parsedUuid = generator(winrt::to_string(projectId));

    mPersistenceService->remove(parsedUuid);
  }
  void RemoveByPath(winrt::hstring path)
  {
    mPersistenceService->remove(winrt::to_string(path));
  }

  void Clear() { mPersistenceService->clear(); }

  bool Contains(winrt::hstring name)
  {
    return mPersistenceService->contains(winrt::to_string(name));
  }

  Windows::Foundation::Collections::IVector<winrt::hstring> ProjectsNames()
  {
    auto projectsNames =
        winrt::single_threaded_observable_vector<winrt::hstring>();

    auto nativeProjectNames = mPersistenceService->projectsNames();

    for (auto projectName : nativeProjectNames) {
      projectsNames.Append(winrt::to_hstring(projectName));
    }

    return projectsNames;
  }

  Windows::Foundation::Collections::IVector<
      PhotobookRuntimeComponent::ProjectMetadataEntry>
  ProjectsList()
  {
    auto projectsList = winrt::single_threaded_observable_vector<
        PhotobookRuntimeComponent::ProjectMetadataEntry>();

    auto projects = mPersistenceService->projectsList();

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
    mPersistenceService->rename(winrt::to_string(newName),
                                winrt::to_string(oldName));
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

    mPersistenceService->save(winrt::to_string(thumbnailsDirectoryName),
                              nativeUnstagedImages, nativesStagedImages,
                              nativeRoots);
  }

  bool HasProjectOpen() { return mPersistenceService->hasProjectOpen(); }

  winrt::hstring CurrentProjectUUID()
  {
    return winrt::to_hstring(
        boost::uuids::to_string(mPersistenceService->currentProjectUUID()));
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

    return mPersistenceService->isSaved(nativeUnstagedImages,
                                        nativesStagedImages, nativeRoots);
  }

  winrt::hstring Hash(winrt::hstring path) { return winrt::hstring(); }

  PhotobookRuntimeComponent::PaperSettings GetPaperSettings()
  {
    return winrt::make<PaperSettings>(
        mPersistenceService->currentProject()->paperSettings);
  }

private:
  std::shared_ptr<PB::PersistenceService> mPersistenceService = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
