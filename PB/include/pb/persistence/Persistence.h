#pragma once

#include <pb/DatabaseService.h>
#include <pb/DurableHashService.h>
#include <pb/image/VirtualImage.h>
#include <pb/persistence/FilePersistence.h>
#include <pb/project/Project.h>

namespace PB {
class PersistenceProjectListener {
public:
  virtual ~PersistenceProjectListener() = default;
  virtual void onProjectRead(
      std::string name, std::shared_ptr<Project> project,
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
      std::vector<Path>                                       &roots) = 0;
  virtual void onProjectPersistenceError(PBDev::Error) = 0;
  virtual void onJsonRead(Json json) = 0;
};

class PersistenceMetadataListener {
public:
  virtual ~PersistenceMetadataListener() = default;
  virtual void onMetadataRead(
      boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata) = 0;
  virtual void onMetadataPersistenceError(PBDev::Error) = 0;
};
/*
class Persistence final {
public:
  static std::optional<PBDev::Error>
  createSupportDirectory(Path path, std::string thumbnailDirectoryName);

  static Json
  serialization(Project project,
                std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                    &unstagedImages,
                std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
                std::vector<Path> const                          &roots);

  void configure(PersistenceProjectListener *);
  void configure(PersistenceMetadataListener *);

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);
  void configureDurableHashService(
      std::shared_ptr<DurableHashService> durableHashService);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void start();

  void persistProject(Path localInstallFolder, std::string name, Json json,
                      std::string thumbnailsDirectoryName);

  void persistMetadata(boost::uuids::uuid const &id, std::string name);

  void recallMetadata();
  void recallProject(Path projectPath);

  void deleteMetadata(std::string id);
  void deleteProject(Path projectFile, std::string thumbnailsDirectoryName,
                     boost::uuids::uuid id);

private:
  void persistProject(Path localInstallFolder, Path filePath, Json json,
                      std::string thumbnailsDirectoryName);

  PersistenceProjectListener  *mPersistenceProjectListener = nullptr;
  PersistenceMetadataListener *mPersistenceMetadataListener = nullptr;

  std::shared_ptr<PlatformInfo>       mPlatformInfo = nullptr;
  std::shared_ptr<DatabaseService>    mDatabaseService = nullptr;
  std::shared_ptr<DurableHashService> mDurableHashService = nullptr;
};
*/
} // namespace PB
