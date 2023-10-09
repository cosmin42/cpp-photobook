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
#include <pb/UnstagedImagesLogic.h>
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
      : mParent(listener), mGalleryListener(std::ref(*this)),
        mGallery(mGalleryListener), mPaperSettings(A4_PAPER)
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
    Context::inst().data().images().clear();
  }

  PaperSettings paperSettings() { return mPaperSettings; }

  void setPaperSettings(PaperSettings paperSettings)
  {
    mPaperSettings = paperSettings;
  }

  void loadProject(Path const &path)
  {
    Persistence<void> projectPersistence(path);
    projectPersistence.load([this, &projectPersistence](
                                std::optional<Error> maybeError) {
      if (!maybeError) {
        auto projectDetailsOrError = PB::convert(projectPersistence.cache());

        if (std::holds_alternative<Error>(projectDetailsOrError)) {
        }
        else {
          auto &projectDetails =
              std::get<ProjectDetails>(projectDetailsOrError);

          mProject = Project<PersistenceType>(projectDetails);

          mUnstagedImagesLogic.provideProjectDetails(projectDetails);
        }
      }
      else {
        mParent.onError(
            Error() << ErrorCode::CouldNotLoadProjectFromGenericPersistence);
      }
    });
  }

  void onPersistenceLoaded() { printDebug("Persistence loaded.\n"); }

  void onError(Error error) { mParent.onError(error); }

  void addMedia(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validInputRootPath(fsPath);
    std::visit(
        overloaded{
            [this](PB::Path const &path) {
              printDebug("Add media %s\n", path.string().c_str());

              auto ptr = std::make_shared<
                  MediaMapListener<PhotoBookListenerType, PersistenceType>>(
                  std::ref(*this));
              mListeners.insert({path, ptr});
              auto listener = mListeners.at(path);
              mMappingJobs.emplace(
                  path, MediaMapper<PhotoBookListenerType, PersistenceType>(
                            path, listener));
            },
            [this](Error error) { mParent.onError(error); }},
        result);
    mMappingJobs.at(fsPath).start();
  }

  void setOutputPath(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validOutputRootPath(fsPath);
    std::visit(overloaded{[this](PB::Path const &) mutable {},
                          [this](Error error) { mParent.doError(error); }},
               result);
  }

  void onNewMediaMap(Path &rootPath, MediaMap &newMediaMap)
  {
    Context::inst().data().images().addGroup(
        rootPath, (unsigned)newMediaMap.map().size());
    Context::inst().data().images().addFullPaths(rootPath, newMediaMap.map());

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

    mUnstagedImagesLogic.generateThumbnails(
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
            Context::inst().data().images().addSmall(input, smallOutput);
            Context::inst().data().images().addMedium(input, mediumOutput);
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

  Gallery<PhotoBookListenerType, PersistenceType> &gallery()
  {
    return mGallery;
  }

  auto loadImage(std::string const &path) -> std::shared_ptr<cv::Mat>
  {
    return mImageReader.read(path);
  }

  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>
  {
    auto image = mImageReader.read(path);
    return Process::resize(size, true)(image);
  }

  void exportAlbum(std::string name, std::string const &destinationPath,
                   std::vector<Path> imagesPaths)
  {
    PB::printDebug("Export image to %s", destinationPath.c_str());
    mExporter.exportImages(name, destinationPath, imagesPaths);
  }

  void discardPhotoBook() { PB::printDebug("Discard Photobook\n"); }

  void savePhotoBook(std::string const &newName)
  {
    Path newPath = newName;
    Path oldPath = mProject.details().parentDirectory / mProject.details().name;
    mProject.details().name = newPath.filename().string();
    mProject.details().parentDirectory = newPath.parent_path();

    mCentralPersistence
        .cache()[boost::uuids::to_string(mProject.details().uuid)] =
        mProject.details().parentDirectory.string() + "\\" +
        mProject.details().name;
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

  std::unordered_map<Path, Path> &thumbnails()
  {
    return Context::inst().data().smallThumbnails();
  }

  void addStagedPhoto(Thumbnails th)
  {
    Context::inst().data().images().stagePhoto(th);
  }

  void insertStagedPhoto(Thumbnails path, int position)
  {
    Context::inst().data().images().stagePhoto(path, position);
  }

  void removeStagedPhoto(int index)
  {
    Context::inst().data().images().unstagePhoto(index);
  }

  std::vector<Thumbnails> &stagedPhotos()
  {
    return Context::inst().data().images().stagedPhotos();
  }

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
  GalleryListener<PhotoBookListenerType, PersistenceType> mGalleryListener;
  Gallery<PhotoBookListenerType, PersistenceType>         mGallery;
  ImageReader                                             mImageReader;
  UnstagedImagesLogic                                     mUnstagedImagesLogic;
  Exporter<Pdf>                                           mExporter;
  int                                                     mProgress = 0;
  PaperSettings                                           mPaperSettings;
};
} // namespace PB