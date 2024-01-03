#pragma once

#include <pb/persistence/FilePersistence.h>
#include <pb/persistence/SQLPersistence.h>
#include <pb/project/Project.h>

namespace PB {
class PersistenceProjectListener {
public:
  virtual void onProjectRead(Project project) = 0;
  virtual void onProjectPersistenceError(PBDev::Error) = 0;
};

class PersistenceMetadataListener {
public:
  virtual void onMetadataRead(ProjectMetadata projectMetadata) = 0;
  virtual void onMetadataRead(std::vector<ProjectMetadata> projectMetadata) = 0;
  virtual void onMetadataPersistenceError(PBDev::Error) = 0;
};

class Persistence final {
public:
  static std::optional<PBDev::Error> createSupportDirectory(Path path);

  explicit Persistence(
      Path                         applicationLocalStatePath,
      PersistenceProjectListener  *persistenceProjectListener,
      PersistenceMetadataListener *persistenceMetadataListener);

  ~Persistence() = default;

  void setPersistenceListener(
      PersistenceProjectListener  *persistenceProjectListener,
      PersistenceMetadataListener *persistenceMetadataListener);

  void persistProject(Path filePath, ProjectSnapshot project);
  void persistMetadata(ProjectMetadata projectMetadata);

  void recallMetadata();
  void recallProject(Path projectPath);

  void deleteMetadata(std::string id);
  void deleteProject(Path projectFile, Path projectFolder);

  bool isSaved(ProjectSnapshot const &projectDetails) const;

private:
  PersistenceProjectListener  *mPersistenceProjectListener;
  PersistenceMetadataListener *mPersistenceMetadataListener;
  SQLitePersistence            mCentral;
  Json                         mProjectCache;
};
} // namespace PB
