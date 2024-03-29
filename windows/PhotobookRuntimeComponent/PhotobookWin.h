#pragma once

#include "PhotobookWin.g.h"

#include "ImageViews.h"
#include "PBError.h"
#include "ProgressInfo.h"
#include "Settings.h"
#include "VirtualImagePtr.h"

#include <pb/PhotoBook.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
class PhotobookListener final : public PB::PhotobookListener {
public:
  explicit PhotobookListener(
      PhotobookRuntimeComponent::PhotobookListener const &managedListener)
      : mManagedListener(managedListener)
  {
  }

  void onProjectRead() override { mManagedListener.OnProjectRead(); }
  void onProjectRenamed() override { mManagedListener.OnProjectRenamed(); }
  void onMetadataUpdated() override { mManagedListener.OnMetadataUpdated(); }
  void onPersistenceError(PBDev::Error error) override
  {
    mManagedListener.OnPersistenceError(winrt::make<PBError>(error));
  }

  void onExportFinished() override { mManagedListener.OnExportFinished(); }
  void onError(PBDev::Error error) override
  {
    mManagedListener.OnError(winrt::make<PBError>(error));
  }
  void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                          int index) override
  {
    Windows::Foundation::Collections::IVector<
        PhotobookRuntimeComponent::VirtualImagePtr>
        managedPhotos = winrt::single_threaded_vector<
            PhotobookRuntimeComponent::VirtualImagePtr>();

    for (auto photo : photos) {
      managedPhotos.Append(winrt::make<VirtualImagePtr>(photo));
    }

    mManagedListener.OnStagedImageAdded(managedPhotos, index);
  }
  void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override
  {
    Windows::Foundation::Collections::IVector<uint32_t> managedPhotos =
        winrt::single_threaded_vector<uint32_t>();

    for (auto index : removedIndexes) {
      managedPhotos.Append(index);
    }

    mManagedListener.OnStagedImageRemoved(managedPhotos);
  }

  void onMappingStarted(Path path) override
  {
    mManagedListener.OnMappingStarted(winrt::to_hstring(path.string()));
  }
  void onMappingFinished(Path path) override
  {
    mManagedListener.OnMappingFinished(winrt::to_hstring(path.string()));
  }
  void onMappingAborted(Path path) override
  {
    mManagedListener.OnMappingAborted(winrt::to_hstring(path.string()));
  }

  void onImageUpdated(Path root, int row, int index) override
  {
    mManagedListener.OnImageUpdated(winrt::to_hstring(root.string()), row,
                                    index);
  }

  void post(std::function<void()> f) override {}

  void onProgressUpdate(PB::ProgressInfo definedProgress,
                        PB::ProgressInfo undefinedProgress) override
  {
    mManagedListener.OnProgressUpdate(
        winrt::make<ProgressInfo>(definedProgress),
        winrt::make<ProgressInfo>(undefinedProgress));
  }

private:
  PhotobookRuntimeComponent::PhotobookListener mManagedListener;
};

struct PhotobookWin : PhotobookWinT<PhotobookWin> {
  PhotobookWin(winrt::hstring localStatePath, winrt::hstring installPath)
  {
    mPhotobook = std::make_shared<PB::Photobook>(
        winrt::to_string(localStatePath), winrt::to_string(installPath));
  }

  ~PhotobookWin()
  {
    if (mPhotobookListener) {
      delete mPhotobookListener;
    }
  }

  winrt::hstring GenerateProjectName()
  {
    auto projectName = PB::Project::generateAlbumName([this](std::string name) {
      return !mPhotobook->project()->contains(name);
    });

    return winrt::to_hstring(projectName);
  }

  void ConfigurePhotobookListener(
      PhotobookRuntimeComponent::PhotobookListener const &listener)
  {
    if (mPhotobookListener) {
      delete mPhotobookListener;
    }
    mPhotobookListener = new PhotobookListener(listener);
    mPhotobook->configure(
        dynamic_cast<PB::PhotobookListener *>(mPhotobookListener));
  }

  void RecallMetadata();
  void RecallProject(winrt::hstring name);

  void NewProject(winrt::hstring name);
  void UnloadProject();

  PhotobookRuntimeComponent::ImageViews GetImageViews()
  {
    return winrt::make<ImageViews>(mPhotobook->imageViews());
  }

  PhotobookRuntimeComponent::Settings GetSettings()
  {
    return winrt::make<Settings>(mPhotobook->project());
  }

  void AddImportFolder(winrt::hstring importPath);
  void RemoveImportFolder(winrt::hstring path);

  void ExportPDFAlbum(winrt::hstring name, winrt::hstring path);
  void ExportPDFLibharu(winrt::hstring name, winrt::hstring path);
  void ExportJPGAlbum(winrt::hstring name, winrt::hstring path);

private:
  std::shared_ptr<PB::Photobook> mPhotobook = nullptr;
  PB::PhotobookListener         *mPhotobookListener = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation

namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PhotobookWin
    : PhotobookWinT<PhotobookWin, implementation::PhotobookWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation