#include <pb/ImportFoldersLogic.h>

#include <pb/util/FileInfo.h>

namespace PB {
void ImportFoldersLogic::configure(ImportFoldersLogicListener *listener)
{
  mListener = listener;
}

void ImportFoldersLogic::configure(ThreadScheduler *scheduler)
{
  mScheduler = scheduler;
}

void ImportFoldersLogic::configure(std::pair<int, int> screenSize)
{
  mThumbnailsProcessor.setScreenSize(screenSize);
}
void ImportFoldersLogic::configure(std::shared_ptr<Project> project)
{
  mThumbnailsProcessor.provideProjectDetails(project);
}

void ImportFoldersLogic::setObserverManager()
{
  setChangeFunction([this](PBDev::Observer *current, PBDev::Observer *other) {
    if (other) {
      for (auto &[key, value] : mMappingJobs) {
        value->dettach(current);
        value->attach(other);
      }
    }
    else {
      PBDev::basicAssert(false);
    }
  });
}

std::optional<PBDev::Error> ImportFoldersLogic::addImportFolder(Path path)
{
  auto errorOrPath = PBDev::FileInfo::validInputRootPath(path);
  if (std::holds_alternative<PBDev::Error>(errorOrPath)) {
    return std::get<PBDev::Error>(errorOrPath);
  }

  setObserverManager();

  auto mapper = std::make_shared<MediaMapper>(path);
  mapper->attach(this);
  mMappingJobs[path] = mapper;

  return std::nullopt;
}

void ImportFoldersLogic::start(Path path) { mMappingJobs.at(path)->start(); }

void ImportFoldersLogic::stop(Path path) {}

void ImportFoldersLogic::stopAll() {}

void ImportFoldersLogic::update(PBDev::ObservableSubject &subject)
{
  if (dynamic_cast<MediaMapper *>(&subject) != nullptr) {
    auto &mediaMap = static_cast<MediaMapper &>(subject);
    if (mediaMap.state() == MediaMapState::Finished) {
      mListener->onMappingFinished(mediaMap.root(),
                                   mediaMap.importedDirectories());
    }
    else if (mediaMap.state() == MediaMapState::Aborted) {
      mListener->onMappingAborted(mediaMap.root());
    }
    else if (mediaMap.state() == MediaMapState::Started) {
      mListener->onMappingStarted(mediaMap.root());
    }
  }
  else {
    PBDev::basicAssert(false);
  }
}

void ImportFoldersLogic::clearJob(Path root) { mMappingJobs.erase(root); }

void ImportFoldersLogic::onImageProcessed(Path root, Path full, Path medium,
                                          Path small, int progress,
                                          int progressCap)
{

  mScheduler->post([this, root, full, medium, small, progress, progressCap]() {
    mListener->onImageProcessed(root, full, medium, small, progress,
                                progressCap);
  });
}

void ImportFoldersLogic::processImages(Path root, std::vector<Path> newFolders)
{
  mThumbnailsProcessor.generateThumbnails(
      newFolders, root.string(),
      [this, root{root}, maxProgress{newFolders.size()}](
          Path full, Path medium, Path small, int position) {
        onImageProcessed(root, full, medium, small, position, (int)maxProgress);
      });
}
} // namespace PB