#pragma once

#include <pb/persistence/FilePersistence.h>
#include <pb/persistence/SQLPersistence.h>
#include <pb/project/Project.h>

namespace PB {
class PersistenceProjectListener {
public:
  virtual ~PersistenceProjectListener() = default;
  virtual void onProjectRead(std::shared_ptr<Project> project) = 0;
  virtual void onProjectPersistenceError(PBDev::Error) = 0;
};

class PersistenceMetadataListener {
public:
  virtual ~PersistenceMetadataListener() = default;
  virtual void onMetadataRead(
      boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata) = 0;
  virtual void onMetadataPersistenceError(PBDev::Error) = 0;
};

class Persistence final {
public:
  static std::optional<PBDev::Error> createSupportDirectory(Path path);

  void configure(Path localStatePath);
  void configure(PersistenceProjectListener *);
  void configure(PersistenceMetadataListener *);

  void persistProject(std::string name, ProjectSnapshot project);

  void persistMetadata(boost::uuids::uuid const &id, Path path);

  void recallMetadata();
  void recallProject(Path projectPath);

  void deleteMetadata(std::string id);
  void deleteProject(Path projectFile);

  bool isSaved(ProjectSnapshot const &projectDetails) const;

private:
  void persistProject(Path filePath, ProjectSnapshot project);

  PersistenceProjectListener  *mPersistenceProjectListener = nullptr;
  PersistenceMetadataListener *mPersistenceMetadataListener = nullptr;
  SQLitePersistence            mCentral;
  Json                         mProjectCache;
  Path                         mLocalStatePath;
};
} // namespace PB
