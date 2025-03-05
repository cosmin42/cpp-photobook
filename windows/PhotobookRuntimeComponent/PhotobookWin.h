#pragma once

#include "PhotobookWin.g.h"

#include "CollageTemplateInfo.g.h"
#include "CollageTemplateInfo.h"
#include "ImageViews.h"
#include "Int32Pair.g.h"
#include "LutIconInfo.g.h"
#include "LutIconInfo.h"
#include "NoirListener.h"
#include "PBError.h"
#include "ProgressInfo.h"
#include "Settings.h"
#include "VirtualImagePtr.g.h"
#include "VirtualImagePtr.h"
#include "WinConversions.h"

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

  [[deprecated]] void onLutAdded(PB::LutIconInfo iconInfo)
  {
    mManagedListener.OnLutAdded(winrt::make<LutIconInfo>(iconInfo));
  }

  void onLutApplied(PBDev::LutApplicationId lutId, PB::GenericImagePtr image,
                    Path thumbnailsPath) override
  {
    auto managedGuid = WinConversions::toManagedGuid(lutId.raw());
    mManagedListener.OnLutApplied(
        managedGuid,
        winrt::make<VirtualImagePtr>(image, thumbnailsPath.string()));
  }

  void onLutAppliedInMemory(PBDev::LutApplicationId  lutId,
                            std::shared_ptr<cv::Mat> image) override
  {
    auto managedGuid = WinConversions::toManagedGuid(lutId.raw());
    mManagedListener.OnLutAppliedInMemory(managedGuid);
  }

  void onProjectRead() override { mManagedListener.OnProjectRead(); }
  void onMetadataUpdated(std::string focusedProjectName) override
  {
    mManagedListener.OnMetadataUpdated(winrt::to_hstring(focusedProjectName));
  }
  void onPersistenceError(PBDev::Error error) override
  {
    mManagedListener.OnPersistenceError(winrt::make<PBError>(error));
  }

  void onExportFinished() override { mManagedListener.OnExportFinished(); }
  void onError(PBDev::Error error) override
  {
    mManagedListener.OnError(winrt::make<PBError>(error));
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
  void onMappingFinished(Path path, unsigned imagesCount) override
  {
    mManagedListener.OnMappingFinished(winrt::to_hstring(path.string()), imagesCount);
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

  void onCollageCreated(PB::GenericImagePtr newImage,
                        Path                thumbnailsPath) override
  {
    mManagedListener.OnCollageCreated(
        winrt::make<VirtualImagePtr>(newImage, thumbnailsPath.string()));
  }

  void onImageMapped(PBDev::ImageToPaperId id, PB::GenericImagePtr image,
                     Path thumbnailsPath)
  {
    auto nativeUuid = id.raw();

    uint64_t data1 = nativeUuid.data[0] << 24 | nativeUuid.data[1] << 16 |
                     nativeUuid.data[2] << 8 | nativeUuid.data[3];

    uint16_t data2 = nativeUuid.data[4] << 8 | nativeUuid.data[5];
    uint16_t data3 = nativeUuid.data[6] << 8 | nativeUuid.data[7];

    GUID existingGuid = {(unsigned long)data1,
                         data2,
                         data3,
                         {nativeUuid.data[8], nativeUuid.data[9],
                          nativeUuid.data[10], nativeUuid.data[11],
                          nativeUuid.data[12], nativeUuid.data[13],
                          nativeUuid.data[14], nativeUuid.data[15]}};

    winrt::guid managedGuid(existingGuid);

    mManagedListener.OnImageMapped(
        managedGuid,
        winrt::make<VirtualImagePtr>(image, thumbnailsPath.string()));
  }

  void onProgressUpdate(PB::ProgressStatus status) override
  {
    mManagedListener.OnProgressUpdate(winrt::make<ProgressInfo>(status));
  }

private:
  PhotobookRuntimeComponent::PhotobookListener mManagedListener;
};

struct PhotobookWin : PhotobookWinT<PhotobookWin> {
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

  winrt::hstring GetThumbnailsPath()
  {
    auto maybeProject =
        mPhotobook->projectManagementService()->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeProject != nullptr);
    auto thumbnailsPath =
        mPhotobook->platformInfo()->projectSupportFolder(maybeProject->first) /
        "thumbnail-images";

    return winrt::to_hstring(thumbnailsPath.string());
  }

  void StartPhotobook() { mPhotobook->startPhotobook(); }

  Windows::Foundation::Collections::IVector<
      PhotobookRuntimeComponent::CollageTemplateInfo>
  CollageTemplatesThumbnailsList()
  {
    auto collageTemplateManager = mPhotobook->collageService();

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
    auto maybeProject =
        mPhotobook->projectManagementService()->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeProject != nullptr);
    // TODO: Fix redundant staged images staged photos.
    auto stagedPhotos = maybeProject->second.stagedImages()->stagedPhotos();

    std::vector<Path> imagesToMerge;

    for (int i = 0; i < (int)images.Size(); ++i) {
      auto hash = stagedPhotos.at((unsigned)images.GetAt(i))->hash();
      auto imagePath = mPhotobook->platformInfo()->thumbnailByHash(
          maybeProject->first, hash);
      imagesToMerge.push_back(imagePath);
    }

    mPhotobook->collageService()->combineImages(collageIndex, imagesToMerge);
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

  void
  ConfigureNoirListener(PhotobookRuntimeComponent::NoirListener const &listener)
  {
    PBDev::basicAssert(mNoirListener == nullptr);
    mNoirListener = new NoirListener(listener);
    mPhotobook->configureNoirListener(
        dynamic_cast<PB::NoirListener *>(mNoirListener));
  }

  PhotobookRuntimeComponent::VirtualImagePtr EmptyImage()
  {
    auto maybeProject =
        mPhotobook->projectManagementService()->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeProject != nullptr);
    auto thumbnailsPath =
        mPhotobook->platformInfo()->projectSupportFolder(maybeProject->first) /
        "thumbnail-images";
    return winrt::make<VirtualImagePtr>(
        mPhotobook->imageFactory()->defaultRegularImage(),
        thumbnailsPath.string());
  }

  winrt::hstring GenerateProjectName()
  {
    auto newProjectName =
        mPhotobook->projectManagementService()->newAlbumName();

    return winrt::to_hstring(newProjectName);
  }

  void MakeCollages();

  void UnloadProject();

  PhotobookRuntimeComponent::ImageViews GetImageViews()
  {
    return winrt::make<ImageViews>(mPhotobook->projectManagementService());
  }

  PhotobookRuntimeComponent::Settings GetSettings()
  {
    return winrt::make<Settings>(mPhotobook->projectManagementService());
  }

  void AddImportFolder(winrt::hstring importPath);
  void RemoveImportFolder(winrt::hstring path);

  void ExportPDFAlbum(winrt::hstring name, winrt::hstring path);
  void ExportPDFLibharu(winrt::hstring name, winrt::hstring path);
  void ExportJPGAlbum(winrt::hstring name, winrt::hstring path);

  void mapImagesToSPL(Windows::Foundation::Collections::IMap<
                      winrt::guid, PhotobookRuntimeComponent::VirtualImagePtr>
                          images);

private:
  std::shared_ptr<PB::Photobook> mPhotobook = nullptr;
  PB::PhotobookListener         *mPhotobookListener = nullptr;
  PB::NoirListener              *mNoirListener = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation

namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct PhotobookWin
    : PhotobookWinT<PhotobookWin, implementation::PhotobookWin> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation