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

std::string ProjectPersistence::name(boost::uuids::uuid uuid)
{
  return mMetadata.left.at(uuid);
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

void ProjectPersistence::remove(boost::uuids::uuid id) {
  Path projectFile = mProject->metadata().projectFile();

  mPersistence.deleteMetadata(boost::uuids::to_string(id));
  mPersistence.deleteProject(projectFile);
}

void ProjectPersistence::remove(Path path) {

}

void ProjectPersistence::clear() { mProject = nullptr; }

} // namespace PB