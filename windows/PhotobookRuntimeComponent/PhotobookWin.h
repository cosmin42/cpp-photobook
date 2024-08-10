#pragma once

#include "PhotobookWin.g.h"

#include "CollageTemplateInfo.g.h"
#include "CollageTemplateInfo.h"
#include "ImageViews.h"
#include "Int32Pair.g.h"
#include "PBError.h"
#include "ProgressInfo.h"
#include "Settings.h"
#include "VirtualImagePtr.g.h"
#include "VirtualImagePtr.h"

#include "ImageMonitorListener.h"
#include "StagedImagesListener.h"

#include <pb/PhotoBook.h>
#include <pb/image/ImageFactory.h>

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

  void onCollageThumbnailsCreated()
  {
    mManagedListener.OnCollageThumbnailsCreated();
  }

  void post(std::function<void()> f) override
  {
    winrt::PhotobookRuntimeComponent::Functor functor = [f]() { f(); };
    mManagedListener.Post(functor);
  }

  void onProgressUpdate(PB::ProgressInfo definedProgress,
                        PB::ProgressInfo undefinedProgress) override
  {
    mManagedListener.OnProgressUpdate(
        winrt::make<ProgressInfo>(definedProgress),
        winrt::make<ProgressInfo>(undefinedProgress));
  }

  void onCollageCreated(unsigned                          index,
                        std::shared_ptr<PB::VirtualImage> newImage) override
  {
    mManagedListener.OnCollageCreated(index,
                                      winrt::make<VirtualImagePtr>(newImage));
  }

private:
  PhotobookRuntimeComponent::PhotobookListener mManagedListener;
};

struct PhotobookWin : PhotobookWinT<PhotobookWin> {
  static PhotobookRuntimeComponent::VirtualImagePtr
  copyImage(PhotobookRuntimeComponent::VirtualImagePtr image)
  {
    auto nativeImagePtr =
        winrt::get_self<
            winrt::PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
            image)
            ->Unwrap();
    return winrt::make<VirtualImagePtr>(
        PB::ImageFactory::inst().copyImage(nativeImagePtr));
  }

  PhotobookRuntimeComponent::VirtualImagePtr
  mapImageToSPL(PhotobookRuntimeComponent::VirtualImagePtr image)
  {
    auto nativeImagePtr =
        winrt::get_self<
            winrt::PhotobookRuntimeComponent::implementation::VirtualImagePtr>(
            image)
            ->Unwrap();
    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    Path projectThumbnailsRoot =
        mPhotobook->platformInfo()->localStatePath / "th" /
        boost::uuids::to_string(mPhotobook->project()->currentProjectUUID());

    return winrt::make<VirtualImagePtr>(
        PB::ImageFactory::inst().mapImageToPaper(
            nativeImagePtr, projectThumbnailsRoot / newImageName));
  }

  static PhotobookRuntimeComponent::PaperSettings
  GetDefaultSettings(PhotobookRuntimeComponent::PaperType paperType)
  {
    switch (paperType) {
    case PhotobookRuntimeComponent::PaperType::A4_Landscape:
      return winrt::make<PaperSettings>(PB::OneConfig::A4_LANDSCAPE_PAPER);
    case PhotobookRuntimeComponent::PaperType::A4_Portrait:
      return winrt::make<PaperSettings>(PB::OneConfig::A4_PORTRAIT_PAPER);
    case PhotobookRuntimeComponent::PaperType::A3_Landscape:
      return winrt::make<PaperSettings>(PB::OneConfig::A3_LANDSCAPE_PAPER);
    case PhotobookRuntimeComponent::PaperType::A3_Portrait:
      return winrt::make<PaperSettings>(PB::OneConfig::A3_PORTRAIT_PAPER);
    case PhotobookRuntimeComponent::PaperType::A5_Landscape:
      return winrt::make<PaperSettings>(PB::OneConfig::A5_LANDSCAPE_PAPER);
    case PhotobookRuntimeComponent::PaperType::A5_Portrait:
      return winrt::make<PaperSettings>(PB::OneConfig::A5_PORTRAIT_PAPER);
    case PhotobookRuntimeComponent::PaperType::Square_Paper:
      return winrt::make<PaperSettings>(PB::OneConfig::SQUARE_PAPER);
    }
    PBDev::basicAssert(false);
    return winrt::make<PaperSettings>(PB::OneConfig::A4_LANDSCAPE_PAPER);
  }

  static PhotobookRuntimeComponent::PaperSettings
  GetDefaultSerializedSettings(winrt::hstring serializedPaperType)
  {
    auto paperType =
        PB::deserializePaperType(winrt::to_string(serializedPaperType));
    if (paperType == PB::PaperType::Custom) {
      PB::PaperSettings defaultPaperSettings;
      defaultPaperSettings.type = PB::PaperType::Custom;
      return winrt::make<PaperSettings>(defaultPaperSettings);
    }
    return GetDefaultSettings((PhotobookRuntimeComponent::PaperType)paperType);
  }

  PhotobookWin(winrt::hstring localStatePath, winrt::hstring installPath,
               PhotobookRuntimeComponent::Int32Pair screenSize)
  {
    auto screenSizeNative = std::pair<unsigned, unsigned>{
        (unsigned)screenSize.First(), (unsigned)screenSize.Second()};
    mPhotobook = std::make_shared<PB::Photobook>(
        winrt::to_string(localStatePath), winrt::to_string(installPath),
        screenSizeNative);
  }

  ~PhotobookWin()
  {
    if (mPhotobookListener) {
      delete mPhotobookListener;
    }
  }

  Windows::Foundation::Collections::IVector<
      PhotobookRuntimeComponent::CollageTemplateInfo>
  CollageTemplatesThumbnailsList()
  {
    auto collageTemplateManager = mPhotobook->collageManager();

    if (collageTemplateManager == nullptr) {
      return winrt::single_threaded_vector<
          PhotobookRuntimeComponent::CollageTemplateInfo>();
    }

    auto collageTemplateInfoList = collageTemplateManager->getTemplatesPaths();

    auto managedPaths = winrt::single_threaded_vector<
        PhotobookRuntimeComponent::CollageTemplateInfo>();

    for (auto collageTemplateInfo : collageTemplateInfoList) {

      managedPaths.Append(
          winrt::make<CollageTemplateInfo>(collageTemplateInfo));
    }
    return managedPaths;
  }

  void MakeCollage(Windows::Foundation::Collections::IVector<unsigned> images,
                   unsigned const collageIndex)
  {
    // TODO: Fix redundant staged images staged photos.
    auto stagedPhotos = mPhotobook->imageViews().stagedImages().stagedPhotos();

    std::vector<Path> imagesToMerge;

    for (int i = 0; i < (int)images.Size(); ++i) {
      auto imagePath =
          stagedPhotos.at((unsigned)images.GetAt(i))->frontend().medium;
      imagesToMerge.push_back(imagePath);
    }

    mPhotobook->collageManager()->combineImages(collageIndex, imagesToMerge);
  }

  void ConfigureStagedImagesListener(
      PhotobookRuntimeComponent::StagedImagesListener const &listener)
  {
    if (mStagedImagesListener) {
      delete mStagedImagesListener;
    }
    mStagedImagesListener =
        new PhotobookRuntimeComponent::implementation::StagedImagesListener(
            listener);
    mPhotobook->configure(
        dynamic_cast<PB::StagedImagesListener *>(mStagedImagesListener));
  }

  void ConfigureImageMonitorListener(
      PhotobookRuntimeComponent::ImageMonitorListener const &listener)
  {
    if (mImageMonitorListener) {
      delete mImageMonitorListener;
    }
    mImageMonitorListener = new ImageMonitorListener(listener);
    mPhotobook->configure(
        dynamic_cast<PB::ImageMonitorListener *>(mImageMonitorListener));
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

  void ConfigureCurrentProject() { mPhotobook->configureCurrentProject(); }

  PhotobookRuntimeComponent::VirtualImagePtr EmptyImage()
  {
    return winrt::make<VirtualImagePtr>(
        PB::ImageFactory::inst().defaultRegularImage());
  }

  winrt::hstring GenerateProjectName()
  {
    auto projectName = PB::Project::generateAlbumName([this](std::string name) {
      return !mPhotobook->project()->contains(name);
    });

    return winrt::to_hstring(projectName);
  }

  void RecallMetadata();
  void RecallProject(winrt::hstring name);

  void NewProject(winrt::hstring                           name,
                  PhotobookRuntimeComponent::PaperSettings paperSettings);
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

  void mapImagesToSPL(Windows::Foundation::Collections::IMap<winrt::guid,
                          PhotobookRuntimeComponent::VirtualImagePtr>
                                        images);

private:
  std::shared_ptr<PB::Photobook> mPhotobook = nullptr;
  PB::PhotobookListener         *mPhotobookListener = nullptr;
  PB::StagedImagesListener      *mStagedImagesListener = nullptr;
  PB::ImageMonitorListener      *mImageMonitorListener = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation

namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PhotobookWin
    : PhotobookWinT<PhotobookWin, implementation::PhotobookWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation