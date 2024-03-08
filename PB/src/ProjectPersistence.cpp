#include <pb/persistence/ProjectPersistence.h>

namespace PB {
ProjectPersistence::ProjectPersistence()
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

void ProjectPersistence::configure(Path localStatePath)
{
  mLocalStatePath = localStatePath;
  mPersistence.configure(localStatePath);
}

void ProjectPersistence::configure(ProjectPersistenceListener *listener)
{
  mListener = listener;
}

std::shared_ptr<Project> ProjectPersistence::currentProject()
{
  return mProject;
}

void ProjectPersistence::recallProject(boost::uuids::uuid const uuid)
{
  auto hashSet = mPersistence.hashSet(uuid);
  for (auto it : hashSet) {
    mCurrentHashes.insert({it.left, formPath(it.right)});
  }
  mPersistence.recallProject(name(uuid));
}

Path ProjectPersistence::hash(Path path)
{
  if (mCurrentHashes.left.find(path) != mCurrentHashes.left.end()) {
    return mCurrentHashes.left.at(path);
  }

  auto hash = mPersistence.hash(path, currentProjectUUID());
  mCurrentHashes.insert({path, formPath(hash)});
  return mCurrentHashes.left.at(path);
}

void ProjectPersistence::recallProject(std::string name)
{
  auto projectPath = mLocalStatePath / (name + Context::BOOK_EXTENSION);

  mPersistence.recallProject(projectPath);
}

void ProjectPersistence::recallMetadata() { mPersistence.recallMetadata(); }

boost::uuids::uuid ProjectPersistence::uuid(std::string name)
{
  return mMetadata.right.at(name);
}

bool ProjectPersistence::hasUUID(std::string name) const
{
  return mMetadata.right.find(name) != mMetadata.right.end();
}

std::vector<std::string> ProjectPersistence::projectsNames() const
{
  std::vector<std::string> projects;
  for (auto &it : mMetadata) {
    projects.push_back(it.right);
  }
  return projects;
}

void ProjectPersistence::newProject(std::string              name,
                                    std::shared_ptr<Project> project)
{
  mProject = project;
  mMetadata.insert({boost::uuids::random_generator()(), name});
  mOpenedUUID = mMetadata.right.at(name);
  auto thumbnailsDirectoryName =
      boost::uuids::to_string(mMetadata.right.at(name));
  save(thumbnailsDirectoryName, {}, {}, {});
}

std::string ProjectPersistence::name(boost::uuids::uuid uuid)
{
  return mMetadata.left.at(uuid);
}

Path ProjectPersistence::formPath(std::string hash)
{
  auto id = currentProjectUUID();
  auto r = mMetadata.left.at(id);

  return VirtualImage::platformInfo->localStatePath / "th" /
         boost::uuids::to_string(currentProjectUUID()) / (hash + ".JPG");
}

Path ProjectPersistence::path(boost::uuids::uuid uuid)
{
  return mLocalStatePath / (mMetadata.left.at(uuid) + Context::BOOK_EXTENSION);
}

void ProjectPersistence::onProjectRead(
    std::string name, std::shared_ptr<Project> project,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
    std::vector<Path>                                       &roots)
{
  mProject = project;
  mOpenedUUID = mMetadata.right.at(name);
  mListener->onProjectRead(unstagedImages, stagedImages, roots);
}

void ProjectPersistence::onProjectPersistenceError(PBDev::Error error)
{
  mListener->onPersistenceError(error);
}

void ProjectPersistence::onMetadataRead(
    boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata)
{
  mMetadata = metadata;
  mListener->onMetadataUpdated();
}

void ProjectPersistence::onMetadataPersistenceError(PBDev::Error error)
{
  mListener->onPersistenceError(error);
}

void ProjectPersistence::onJsonRead(Json json)
{
  mJson = json;
  std::string loadedJson = mJson.dump();
}

void ProjectPersistence::remove(boost::uuids::uuid id)
{
  mPersistence.deleteMetadata(boost::uuids::to_string(id));
  mPersistence.deleteProject(path(id), boost::uuids::to_string(id), id);
}

void ProjectPersistence::remove(Path path) {}

void ProjectPersistence::clear()
{
  mJson.clear();
  mProject = nullptr;
}

bool ProjectPersistence::contains(std::string name) const
{
  return mMetadata.right.find(name) != mMetadata.right.end();
}

std::vector<std::tuple<boost::uuids::uuid, std::string, Path>>
ProjectPersistence::projectsList() const
{
  std::vector<std::tuple<boost::uuids::uuid, std::string, Path>> projects;
  for (auto const &it : mMetadata) {
    projects.push_back(
        {it.left, it.right,
         mLocalStatePath / (it.right + Context::BOOK_EXTENSION)});
  }
  return projects;
}

void ProjectPersistence::rename(std::string newName, std::string oldName)
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

    auto newProjectPath = mLocalStatePath / (newName + Context::BOOK_EXTENSION);
    auto oldProjectPath = mLocalStatePath / (oldName + Context::BOOK_EXTENSION);
    std::filesystem::rename(oldProjectPath, newProjectPath);

    mListener->onMetadataUpdated();
  }
}

boost::uuids::uuid ProjectPersistence::currentProjectUUID() const
{
  return mOpenedUUID.value();
}

bool ProjectPersistence::hasProjectOpen() const
{
  return mOpenedUUID.has_value();
}

bool ProjectPersistence::isSaved(
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  auto currentJson = Persistence::serialization(
      mProject->active(), unstagedImages, stagedImages, roots);
  if (currentJson.is_null()) {
    PB::printDebug("Current NULL\n");
  }
  std::string currentJsonDump = currentJson.dump();

  std::string diskJsonDump = mJson.dump();

  return currentJson == mJson;
}

void ProjectPersistence::save(
    std::string thumbnailsDirectoryName,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  PBDev::basicAssert(mProject != nullptr);
  PBDev::basicAssert(mOpenedUUID.has_value());

  mJson = Persistence::serialization(mProject->active(), unstagedImages,
                                     stagedImages, roots);

  auto const &name = mMetadata.left.find(mOpenedUUID.value())->second;
  mPersistence.persistProject(name, mJson, thumbnailsDirectoryName);
  mPersistence.persistMetadata(mOpenedUUID.value(), name);
}

} // namespace PB