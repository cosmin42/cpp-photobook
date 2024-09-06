#include <pb/persistence/PersistenceService.h>

namespace PB {
PersistenceService::PersistenceService(
    std::shared_ptr<PlatformInfo> platformInfo)
    : mPlatformInfo(platformInfo)
{
  auto persistenceMetadataListener =
      dynamic_cast<PersistenceMetadataListener *>(this);
  PBDev::basicAssert(persistenceMetadataListener != nullptr);
  mPersistence.configure(persistenceMetadataListener);

  auto persistenceProjectListener =
      dynamic_cast<PersistenceProjectListener *>(this);
  PBDev::basicAssert(persistenceProjectListener != nullptr);
  mPersistence.configure(persistenceProjectListener);
}

void PersistenceService::configure(PersistenceServiceListener *listener)
{
  mListener = listener;
}

void PersistenceService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
  mPersistence.configurePlatformInfo(platformInfo);
}

std::shared_ptr<Project> PersistenceService::currentProject()
{
  return mProject;
}

void PersistenceService::recallProject(boost::uuids::uuid const uuid)
{
  /*
  auto hashSet = mPersistence.hashSet(uuid);
  PBDev::basicAssert(hashSet.size() == 1);
  for (auto it : hashSet) {
    mCurrentHashes.insert({it.left, formPath(it.right)});
  }
  mPersistence.recallProject(name(uuid));
  */
}

/*
Path PersistenceService::hash(Path path)
{
  if (mCurrentHashes.left.find(path) != mCurrentHashes.left.end()) {
    return mCurrentHashes.left.at(path);
  }

  auto hash = mPersistence.hash(path, currentProjectUUID());
  mCurrentHashes.insert({path, formPath(hash)});
  return mCurrentHashes.left.at(path);
}
*/

void PersistenceService::recallProject(std::string name)
{
  auto projectPath =
      mPlatformInfo->localStatePath / (name + OneConfig::BOOK_EXTENSION);

  mPersistence.recallProject(projectPath);
}

void PersistenceService::recallMetadata() { mPersistence.recallMetadata(); }

boost::uuids::uuid PersistenceService::uuid(std::string name)
{
  return mMetadata.right.at(name);
}

bool PersistenceService::hasUUID(std::string name) const
{
  return mMetadata.right.find(name) != mMetadata.right.end();
}

std::vector<std::string> PersistenceService::projectsNames() const
{
  std::vector<std::string> projects;
  for (auto &it : mMetadata) {
    projects.push_back(it.right);
  }
  return projects;
}

void PersistenceService::newProject(std::string              name,
                                    std::shared_ptr<Project> project)
{
  mProject = project;
  auto projectId = boost::uuids::random_generator()();
  mMetadata.insert({projectId, name});
  mOpenedUUID = mMetadata.right.at(name);
  auto thumbnailsDirectoryName =
      boost::uuids::to_string(mMetadata.right.at(name));
  save(thumbnailsDirectoryName, {}, {}, {});
}

std::string PersistenceService::name(boost::uuids::uuid uuid)
{
  return mMetadata.left.at(uuid);
}

Path PersistenceService::formPath(std::string hash)
{
  auto id = currentProjectUUID();
  auto r = mMetadata.left.at(id);

  return mPlatformInfo->localStatePath /
         boost::uuids::to_string(currentProjectUUID()) / (hash + ".JPG");
}

Path PersistenceService::path(boost::uuids::uuid uuid)
{
  return mPlatformInfo->localStatePath /
         (mMetadata.left.at(uuid) + OneConfig::BOOK_EXTENSION);
}

void PersistenceService::onProjectRead(
    std::string name, std::shared_ptr<Project> project,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
    std::vector<Path>                                       &roots)
{
  mProject = project;
  mOpenedUUID = mMetadata.right.at(name);
  mListener->onProjectRead(unstagedImages, stagedImages, roots);
}

void PersistenceService::onProjectPersistenceError(PBDev::Error error)
{
  mListener->onPersistenceError(error);
}

void PersistenceService::onMetadataRead(
    boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata)
{
  mMetadata = metadata;
  mListener->onMetadataUpdated();
}

void PersistenceService::onMetadataPersistenceError(PBDev::Error error)
{
  mListener->onPersistenceError(error);
}

void PersistenceService::onJsonRead(Json json)
{
  mJson = json;
  std::string loadedJson = mJson.dump();
}

void PersistenceService::remove(boost::uuids::uuid id)
{
  mPersistence.deleteMetadata(boost::uuids::to_string(id));
  mPersistence.deleteProject(path(id), boost::uuids::to_string(id), id);
}

void PersistenceService::remove(Path path) {}

void PersistenceService::clear()
{
  mJson.clear();
  mProject = nullptr;
}

bool PersistenceService::contains(std::string name) const
{
  return mMetadata.right.find(name) != mMetadata.right.end();
}

std::vector<std::tuple<boost::uuids::uuid, std::string, Path>>
PersistenceService::projectsList() const
{
  std::vector<std::tuple<boost::uuids::uuid, std::string, Path>> projects;
  for (auto const &it : mMetadata) {
    projects.push_back({it.left, it.right,
                        mPlatformInfo->localStatePath /
                            (it.right + OneConfig::BOOK_EXTENSION)});
  }
  return projects;
}

void PersistenceService::rename(std::string newName, std::string oldName)
{

  if (oldName.empty()) {
    PBDev::basicAssert(currentProject() != nullptr);

    PBDev::basicAssert(mOpenedUUID.has_value());

    PBDev::basicAssert(mMetadata.left.find(mOpenedUUID.value()) !=
                       mMetadata.left.end());

    oldName = mMetadata.left.find(mOpenedUUID.value())->second;
  }

  if (newName != oldName) {
    bool success =
        mMetadata.right.replace_key(mMetadata.right.find(oldName), newName);
    PBDev::basicAssert(success);

    auto &uuid = mMetadata.right.at(newName);
    mPersistence.persistMetadata(uuid, newName);

    auto newProjectPath =
        mPlatformInfo->localStatePath / (newName + OneConfig::BOOK_EXTENSION);
    auto oldProjectPath =
        mPlatformInfo->localStatePath / (oldName + OneConfig::BOOK_EXTENSION);
    std::filesystem::rename(oldProjectPath, newProjectPath);

    mListener->onMetadataUpdated();
  }
}

boost::uuids::uuid PersistenceService::currentProjectUUID() const
{
  return mOpenedUUID.value();
}

bool PersistenceService::hasProjectOpen() const
{
  return mOpenedUUID.has_value();
}

bool PersistenceService::isSaved(
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  auto project = *(mProject.get());
  auto currentJson =
      Persistence::serialization(project, unstagedImages, stagedImages, roots);
  std::string currentJsonDump = currentJson.dump();

  std::string diskJsonDump = mJson.dump();

  return currentJson == mJson;
}

void PersistenceService::save(
    std::string thumbnailsDirectoryName,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  PBDev::basicAssert(mProject != nullptr);
  PBDev::basicAssert(mOpenedUUID.has_value());

  auto project = *(mProject.get());
  mJson =
      Persistence::serialization(project, unstagedImages, stagedImages, roots);

  auto const &name = mMetadata.left.find(mOpenedUUID.value())->second;
  mPersistence.persistProject(mPlatformInfo->localStatePath, name, mJson,
                              thumbnailsDirectoryName);
  mPersistence.persistMetadata(mOpenedUUID.value(), name);
}

} // namespace PB