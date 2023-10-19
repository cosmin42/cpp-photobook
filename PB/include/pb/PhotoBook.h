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

template <typename PhotoBookListenerType>
  requires PhotoBookListenerConcept<PhotoBookListenerType>
class PhotoBook final {
public:
  PhotoBook(PhotoBookListenerType &listener, Path centralPersistencePath,
            std::pair<int, int> screenSize)
      : mParent(listener), mCentralPersistencePath(centralPersistencePath),
        mCentralPersistence(mCentralPersistencePath),
        mThumbnailsProcessor(screenSize), mPaperSettings(A4_PAPER)
  {
    printDebug("Photobook created.\n");

    auto maybeError = mCentralPersistence.connect();
    if (maybeError) {
      onError(maybeError.value());
    }
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
    FilePersistence projectPersistence(path);
    projectPersistence.read(
        [this, onReturn](std::variant<Json, Error> jsonOrError) {
          if (std::holds_alternative<Error>(jsonOrError)) {
            auto error = std::get<Error>(jsonOrError);
            onReturn(error);
          }
          else {
            auto &json = std::get<Json>(jsonOrError);

            auto projectDetailsOrError = ProjectDetails::parse(json);

            if (std::holds_alternative<Error>(projectDetailsOrError)) {
              onReturn(std::get<Error>(projectDetailsOrError));
            }
            else {
              auto &projectDetails =
                  std::get<ProjectDetails>(projectDetailsOrError);

              mProject = Project(projectDetails);

              mThumbnailsProcessor.provideProjectDetails(projectDetails);

              auto importedFolders = mProject.details().importedFolderList();
              for (auto &path : importedFolders) {
                addImportFolder(path);
              }
              auto stagedImages = mProject.details().stagedImagesList();

              for (auto i = 0; i < stagedImages.size(); ++i) {
                addStagedPhoto(Thumbnails(stagedImages.at(i)));
              }

              onReturn(std::nullopt);
            }
          }
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

      auto &rootFolders = imageSupport().groups();
      if (rootFolders.find(path) != rootFolders.end()) {
        mParent.onError(Error() << ErrorCode::FolderAlreadyImported
                                << "Folder already imported.");
        return;
      }

      auto ptr = std::make_shared<MediaMapListener<PhotoBookListenerType>>(
          std::ref(*this));
      mListeners.insert({path, ptr});
      auto listener = mListeners.at(path);
      mMappingJobs.emplace(path,
                           MediaMapper<PhotoBookListenerType>(path, listener));
      mMappingJobs.at(importPath).start();
    }
  }

  void onImportFolderMapped(Path &rootPath, std::vector<Path> &newMediaMap)
  {
    mImagePaths.addGroup(rootPath);
    mImagePaths.addFullPaths(rootPath, newMediaMap);

    mParent.onAddingUnstagedImagePlaceholder((unsigned)newMediaMap.size());

    mParent.onMappingFinished(rootPath);

    std::vector<std::future<void>> v;

    auto supportDirectoryPath = mProject.details().parentDirectory() /
                                mProject.details().supportDirName();
    if (!std::filesystem::exists(supportDirectoryPath)) {
      if (!FilePersistence::createDirectory(supportDirectoryPath)) {
        mParent.onFinished(rootPath);
        return;
      }
    }

    if (newMediaMap.empty()) {
      mParent.onFinished(rootPath);
    }

    auto start = std::chrono::high_resolution_clock::now();

    mProgress[rootPath] = 0;
    const int maxProgress = (int)newMediaMap.size();

    auto importFolderIndex = mImagePaths.groups().at(rootPath);

    auto groupIdentifier = std::to_string(importFolderIndex) + "sep";

    mThumbnailsProcessor.generateThumbnails(
        newMediaMap, groupIdentifier,
        [this, rootPath{rootPath}, start{start}, maxProgress{maxProgress}](
            Path input, Path smallOutput, Path mediumOutput, int position) {
          mParent.onProgressUpdate(rootPath, mProgress.at(rootPath),
                                   (int)maxProgress);

          mParent.onUnstagedImageAdded(rootPath, input, mediumOutput,
                                       smallOutput, position);

          mParent.post([this, rootPath{rootPath}, input{input},
                        smallOutput{smallOutput}, start{start},
                        mediumOutput{mediumOutput},
                        maxProgress{maxProgress}]() {
            mImagePaths.addSmallPath(input, smallOutput);
            mImagePaths.addMediumPath(input, mediumOutput);
            mProgress[rootPath]++;
            if (mProgress.at(rootPath) == maxProgress) {

              auto end = std::chrono::high_resolution_clock::now();
              std::chrono::duration<double> duration = end - start;
              PB::printDebug("Duration: %f\n", duration.count());

              mParent.onFinished(rootPath);
              mMappingJobs.erase(rootPath);
            }
          });
        });
  }

  void onError(Error error) { mParent.onError(error); }

  Gallery &gallery() { return mGallery; }

  std::optional<PB::Path> selectedImportFolder()
  {
    auto selectedIndex = gallery().selectedIndex();
    if (selectedIndex > -1) {
      return imageSupport().groupByIndex(selectedIndex);
    }
    else {
      return std::nullopt;
    }
  }

  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>
  {
    auto image = mImageReader.read(path);
    return Process::resize(size, true)(image);
  }

  template <typename T> Exporter<T> &exporter();

  template <> Exporter<Pdf> &exporter() { return mExporter; }

  void discardPhotoBook() { PB::printDebug("Discard Photobook\n"); }

  void savePhotoBook() { savePhotoBook(mProject.details().projectFile()); }

  void savePhotoBook(Path newPath)
  {
    bool newSaveFile = false;
    Path oldProjectFile = mProject.details().projectFile();
    Path oldSupportFolder = mProject.details().supportFolder();

    if (newPath != oldProjectFile) {
      mProject.updateProjectPath(newPath.parent_path());
      mProject.updateProjectName(newPath.stem().string());
      newSaveFile = true;

      std::pair<std::string, std::string> entry = {
          boost::uuids::to_string(mProject.details().uuid()),
          mProject.details().projectFile().string()};

      mCentralPersistence.write(entry, [this, newPath{Path(newPath)}](
                                           std::optional<Error> maybeError) {
        if (maybeError) {
          mParent.onError(Error() << ErrorCode::CannotSaveFile);
        }
      });
    }

    auto &groupsSet = imageSupport().groups();

    std::vector<Path> goups;
    for (auto &path : groupsSet) {
      goups.push_back(path.first);
    }
    mProject.details().setImportedPaths(goups);

    auto it = imageSupport().stagedIterator();

    mProject.details().setStagedImages(imageSupport().stagedPhotos());

    auto projectDetailsJson = Json(mProject.details());

    FilePersistence persistence(newPath);

    persistence.write(projectDetailsJson,
                      [this, oldProjectFile, oldSupportFolder,
                       newSaveFile](std::optional<Error> maybeError) {
                        if (maybeError) {
                          mParent.onError(maybeError.value());
                        }
                        else {
                          if (newSaveFile) {
                            std::filesystem::remove(oldSupportFolder);
                            std::filesystem::remove(oldProjectFile);
                          }
                        }
                      });

    PB::printDebug("Save Photobook %s\n", newPath.string().c_str());
  }

  void addStagedPhoto(Thumbnails th)
  {
    mImagePaths.stagePhoto(th);
    mParent.onStagedImageAdded(th);
  }

  void deleteStagedPhoto(std::vector<int> positions)
  {
    mImagePaths.unstagePhoto(positions);
    mParent.onStagedImageRemoved(positions);
  }

  void insertStagedPhoto(Thumbnails path, int position)
  {
    mImagePaths.stagePhoto(path, position);
  }

  void removeStagedPhoto(int index) { mImagePaths.unstagePhoto(index); }

  std::vector<Thumbnails> &stagedPhotos() { return mImagePaths.stagedPhotos(); }

  ImageSupport &imageSupport() { return mImagePaths; }

  bool projectDefaultSaved()
  {
    auto projectParentPath = mProject.details().parentDirectory().string();

    if (projectParentPath.find(mCentralPersistencePath.string()) ==
        std::string::npos) {
      return false;
    }
    return true;
  }

private:
  PhotoBookListenerType &mParent;
  Path                   mCentralPersistencePath;
  SQLitePersistence      mCentralPersistence;
  Project                mProject;
  std::unordered_map<Path,
                     std::shared_ptr<MediaMapListener<PhotoBookListenerType>>>
                                                               mListeners;
  std::unordered_map<Path, MediaMapper<PhotoBookListenerType>> mMappingJobs;
  ImageSupport                                                 mImagePaths;
  Gallery                                                      mGallery;
  ImageReader                                                  mImageReader;
  ThumbnailsProcessor           mThumbnailsProcessor;
  Exporter<Pdf>                 mExporter;
  std::unordered_map<Path, int> mProgress;
  PaperSettings                 mPaperSettings;
};
} // namespace PB