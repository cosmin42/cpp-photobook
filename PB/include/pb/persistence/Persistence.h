#pragma once

#include <pb/FilePersistence.h>
#include <pb/Project.h>
#include <pb/SQLPersistence.h>

namespace PB {
class PersistenceProjectListener {
public:
  virtual void onProjectRead(Project project) = 0;
  virtual void onProjectPersistenceError(Error) = 0;
};

class PersistenceMetadataListener {
public:
  virtual void onMetadataRead(ProjectMetadata projectMetadata) = 0;
  virtual void onMetadataRead(std::vector<ProjectMetadata> projectMetadata) = 0;
  virtual void onMetadataPersistenceError(Error) = 0;
};

class Persistence final {
public:
  explicit Persistence(
      Path                         centralPersistencePath,
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

  bool isSaved(ProjectSnapshot const &projectDetails) const;

  std::optional<Error> createSupportDirectory(Path path);

private:
  PersistenceProjectListener  *mPersistenceProjectListener;
  PersistenceMetadataListener *mPersistenceMetadataListener;
  SQLitePersistence            mCentral;
  Json                         mProjectCache;
};
} // namespace PB
