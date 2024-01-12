#include <pb/persistence/ProjectPersistence.h>

namespace PB {
ProjectPersistence::ProjectPersistence()
{
  mPersistence.configure((PersistenceMetadataListener *)this);
  mPersistence.configure((PersistenceProjectListener *)this);
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

void ProjectPersistence::recallProject(boost::uuids::uuid const &uuid)
{
  mPersistence.recallProject(name(uuid));
}

void ProjectPersistence::recallProject(std::string name)
{
  mPersistence.recallProject(name);
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
  save();
}

std::string ProjectPersistence::name(boost::uuids::uuid uuid)
{
  return mMetadata.left.at(uuid);
}

Path ProjectPersistence::path(boost::uuids::uuid uuid)
{
  return mLocalStatePath / (mMetadata.left.at(uuid) + Context::BOOK_EXTENSION);
}

void ProjectPersistence::onProjectRead(std::shared_ptr<Project> project)
{
  mProject = project;
  mListener->onProjectRead();
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

void ProjectPersistence::remove(boost::uuids::uuid id)
{
  mPersistence.deleteMetadata(boost::uuids::to_string(id));
  mPersistence.deleteProject(path(id));
}

void ProjectPersistence::remove(Path path) {}

void ProjectPersistence::clear() { mProject = nullptr; }

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

    PBDev::basicAssert(mMetadata.left.find(mOpenedUUID.value()) !=
                       mMetadata.left.end());

    oldName = mMetadata.left.find(mOpenedUUID.value())->second;
  }

  bool success =
      mMetadata.right.replace_key(mMetadata.right.find(oldName), newName);
  PBDev::basicAssert(success);

  auto newProjectPath = mLocalStatePath / (newName + Context::BOOK_EXTENSION);

  mPersistence.persistMetadata(mOpenedUUID.value(), newProjectPath);

  if (newName != oldName) {
    auto oldProjectPath = mLocalStatePath / (oldName + Context::BOOK_EXTENSION);
    std::filesystem::rename(oldProjectPath, newProjectPath);
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

void ProjectPersistence::save()
{
  PBDev::basicAssert(mProject != nullptr);
  mProject->sync();

  auto const &name = mMetadata.left.find(mOpenedUUID.value())->second;

  mPersistence.persistProject(name, mProject->active());
}

} // namespace PB