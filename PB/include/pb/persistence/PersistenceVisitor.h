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

class PersistenceVisitor final {
public:
  explicit PersistenceVisitor(
      Path                         centralPersistencePath,
      PersistenceProjectListener  *persistenceProjectListener,
      PersistenceMetadataListener *persistenceMetadataListener);

  ~PersistenceVisitor() = default;

  void persist(Path filePath, ProjectDetails project);
  void persist(ProjectMetadata projectMetadata);

  void recallMetadata();
  void recallProject(Path projectPath);

  void deleteMetadata(std::string id);

  bool isSaved(Json serialization) const;

private:
  PersistenceProjectListener               *mPersistenceProjectListener;
  PersistenceMetadataListener              *mPersistenceMetadataListener;
  SQLitePersistence                         mCentral;
  Json                                      mProjectCache;
};
} // namespace PB
