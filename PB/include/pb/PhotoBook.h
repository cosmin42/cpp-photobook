#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <thread_pool/thread_pool.h>

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/Exporter.h>
#include <pb/FileMapper.h>
#include <pb/Gallery.h>
#include <pb/ImageReader.h>
#include <pb/ImageSetWriter.h>
#include <pb/Pdf.h>
#include <pb/Project.h>
#include <pb/ThumbnailsProcessor.h>
#include <pb/common/Log.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

enum class PaperType { None, A4, A5, A3, Custom };

struct PaperSettings {
  PaperType type;
  int       ppi;
  int       width;
  int       height;
};

static constexpr PaperSettings A4_PAPER = {PaperType::A4, 300, 3508, 2480};
static constexpr PaperSettings A5_PAPER = {PaperType::A5, 300, 2480, 1748};
static constexpr PaperSettings A3_PAPER = {PaperType::A3, 300, 4961, 3508};

template <typename PhotoBookListenerType, typename PersistenceType>
  requires PhotoBookListenerConcept<PhotoBookListenerType>
class PhotoBook final {
public:
  PhotoBook(PhotoBookListenerType &listener)
      : mParent(listener), mGallery(mImagePaths.groups()),
        mPaperSettings(A4_PAPER)
  {
    printDebug("Photobook created.\n");

    mCentralPersistence.load([this](std::optional<Error> maybeError) {
      if (maybeError) {
        onError(maybeError.value());
      }
    });
  }
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook()
  {
    printDebug("Photobook destructed.\n");
    mImagePaths.clear();
  }

  PaperSettings paperSettings() { return mPaperSettings; }

  void setPaperSettings(PaperSettings paperSettings)
  {
    mPaperSettings = paperSettings;
  }

  void loadProject(Path const                               &path,
                   std::function<void(std::optional<Error>)> onReturn)
  {
    Persistence<void> projectPersistence(path);
    projectPersistence.load([this, &projectPersistence,
                             onReturn](std::optional<Error> maybeError) {
      if (!maybeError) {
        auto projectDetailsOrError = PB::convert(projectPersistence.cache());

        if (std::holds_alternative<Error>(projectDetailsOrError)) {
        }
        else {
          auto &projectDetails =
              std::get<ProjectDetails>(projectDetailsOrError);

          mProject = Project<PersistenceType>(projectDetails);

          mThumbnailsProcessor.provideProjectDetails(projectDetails);
        }
      }
      onReturn(maybeError);
    });
  }

  void addImportFolder(Path importPath)
  {
    auto errorOrPath = FileInfo::validInputRootPath(importPath);
    if (std::holds_alternative<Error>(errorOrPath)) {
      mParent.onError(std::get<Error>(errorOrPath));
    }
    else {
      auto path = std::get<Path>(errorOrPath);
      printDebug("Add Input folder %s\n", path.string().c_str());

      auto ptr = std::make_shared<
          MediaMapListener<PhotoBookListenerType, PersistenceType>>(
          std::ref(*this));
      mListeners.insert({path, ptr});
      auto listener = mListeners.at(path);
      mMappingJobs.emplace(
          path,
          MediaMapper<PhotoBookListenerType, PersistenceType>(path, listener));
      mMappingJobs.at(importPath).start();
    }
  }

  void onImportFolderMapped(Path &rootPath, MediaMap &newMediaMap)
  {
    mImagePaths.addGroup(rootPath, (unsigned)newMediaMap.map().size());
    mImagePaths.addFullPaths(rootPath, newMediaMap.map());

    mParent.onAddingUnstagedImagePlaceholder(
        (unsigned)newMediaMap.map().size());

    std::vector<std::future<void>> v;

    if (!Persistence<void>::createDirectory(mProject.details().parentDirectory /
                                            mProject.details().dirName)) {
      mParent.onFinished();
      return;
    }

    if (newMediaMap.map().empty()) {
      mParent.onFinished();
    }

    auto start = std::chrono::high_resolution_clock::now();

    mProgress = 0;
    const int maxProgress = (int)newMediaMap.map().size();

    mThumbnailsProcessor.generateThumbnails(
        newMediaMap.map(),
        [this, rootPath{rootPath}, start{start}, maxProgress{maxProgress}](
            Path input, Path smallOutput, Path mediumOutput, int position) {
          mParent.onProgressUpdate((int)mProgress, (int)maxProgress);

          mParent.onUnstagedImageAdded(input, mediumOutput, smallOutput,
                                       position);

          mParent.post([this, rootPath{rootPath}, input{input},
                        smallOutput{smallOutput}, start{start},
                        mediumOutput{mediumOutput},
                        maxProgress{maxProgress}]() {
            mImagePaths.addSmall(input, smallOutput);
            mImagePaths.addMedium(input, mediumOutput);
            mProgress++;
            if (mProgress == maxProgress) {

              auto end = std::chrono::high_resolution_clock::now();
              std::chrono::duration<double> duration = end - start;
              PB::printDebug("Duration: %f\n", duration.count());
              mParent.onFinished();
            }
          });
        });
    // mMappingJobs.erase(path);
    // mListeners.erase(path);
  }

  void onError(Error error) { mParent.onError(error); }

  Gallery &gallery() { return mGallery; }

  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>
  {
    auto image = mImageReader.read(path);
    return Process::resize(size, true)(image);
  }

  template <typename T> Exporter<T> &exporter();

  template <> Exporter<Pdf> &exporter() { return mExporter; }

  void discardPhotoBook() { PB::printDebug("Discard Photobook\n"); }

  void savePhotoBook(std::string const &newName)
  {
    Path newPath = newName;
    Path oldPath = mProject.details().parentDirectory / mProject.details().name;
    mProject.details().name = newPath.filename().string();
    mProject.details().parentDirectory = newPath.parent_path();

    mCentralPersistence
        .cache()[boost::uuids::to_string(mProject.details().uuid)] =
        (mProject.details().parentDirectory / mProject.details().name).string();
    mCentralPersistence.write([](std::optional<Error> maybeError) {
      if (maybeError) {
        PB::printError("Error saving.\n");
      }
    });

    auto projectDetailsMap =
        std::unordered_map<std::string, std::string>(mProject.details());

    Persistence<void> persistence(newPath);
    persistence.cache().insert(projectDetailsMap.begin(),
                               projectDetailsMap.end());

    std::filesystem::remove(oldPath);

    PB::printDebug("Save Photobook %s\n", newName.c_str());
  }

  void addStagedPhoto(Thumbnails th) { mImagePaths.stagePhoto(th); }

  void insertStagedPhoto(Thumbnails path, int position)
  {
    mImagePaths.stagePhoto(path, position);
  }

  void removeStagedPhoto(int index) { mImagePaths.unstagePhoto(index); }

  std::vector<Thumbnails> &stagedPhotos() { return mImagePaths.stagedPhotos(); }

  ImageSupport &imageSupport() { return mImagePaths; }

private:
  PhotoBookListenerType       &mParent;
  Persistence<PersistenceType> mCentralPersistence;
  Project<PersistenceType>     mProject;
  std::unordered_map<
      Path,
      std::shared_ptr<MediaMapListener<PhotoBookListenerType, PersistenceType>>>
      mListeners;
  std::unordered_map<Path, MediaMapper<PhotoBookListenerType, PersistenceType>>
                      mMappingJobs;
  ImageSupport        mImagePaths;
  Gallery             mGallery;
  ImageReader         mImageReader;
  ThumbnailsProcessor mThumbnailsProcessor;
  Exporter<Pdf>       mExporter;
  int                 mProgress = 0;
  PaperSettings       mPaperSettings;
};
} // namespace PB