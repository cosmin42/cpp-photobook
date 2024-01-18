#pragma once

#include <pb/persistence/FilePersistence.h>
#include <pb/persistence/SQLPersistence.h>
#include <pb/project/Project.h>

namespace PB {
class PersistenceProjectListener {
public:
  virtual ~PersistenceProjectListener() = default;
  virtual void onProjectRead(
      std::string name, std::shared_ptr<Project> project,
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>> &stagedImages) = 0;
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

  void persistProject(
      std::string name, ProjectSnapshot project,
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                       &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
      std::vector<Path> const& roots);

  void persistMetadata(boost::uuids::uuid const &id, std::string name);

  void recallMetadata();
  void recallProject(Path projectPath);

  void deleteMetadata(std::string id);
  void deleteProject(Path projectFile);

  bool isSaved(ProjectSnapshot const &projectDetails) const;

private:
  void
  persistProject(Path filePath, ProjectSnapshot project,
                 std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                     &unstagedImages,
                 std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
                 std::vector<Path> const                          &roots);

  PersistenceProjectListener  *mPersistenceProjectListener = nullptr;
  PersistenceMetadataListener *mPersistenceMetadataListener = nullptr;
  SQLitePersistence            mCentral;
  Json                         mProjectCache;
  Path                         mLocalStatePath;
};
} // namespace PB
