#pragma once
#include <pb/persistence/Persistence.h>

namespace PB {
class ProjectPersistenceListener {
public:
  virtual ~ProjectPersistenceListener() = default;
  virtual void onMetadataUpdated() = 0;
  virtual void onProjectRead(
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
      std::vector<Path>                                       &roots) = 0;
  virtual void onProjectRenamed() = 0;
  virtual void onPersistenceError(PBDev::Error) = 0;
};

class ProjectPersistence final : public PersistenceMetadataListener,
                                 public PersistenceProjectListener {
public:
  ProjectPersistence();
  ~ProjectPersistence() = default;

  void configure(Path localStatePath);
  void configure(ProjectPersistenceListener *listener);

  std::shared_ptr<Project> currentProject();

  void recallProject(boost::uuids::uuid const &uuid);

  void recallProject(std::string name);

  void recallMetadata();

  boost::uuids::uuid uuid(std::string name);

  bool hasUUID(std::string name) const;

  std::vector<std::string> projectsNames() const;

  void newProject(std::string name, std::shared_ptr<Project> project);

  void onProjectRead(
      std::string name, std::shared_ptr<Project> project,
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
      std::vector<Path>                                       &roots) override;
  void onProjectPersistenceError(PBDev::Error) override;

  void onMetadataRead(
      boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata) override;
  void onMetadataPersistenceError(PBDev::Error) override;

  void remove(boost::uuids::uuid id);
  void remove(Path path);

  void clear();

  bool contains(std::string name) const;

  std::vector<std::tuple<boost::uuids::uuid, std::string, Path>>
  projectsList() const;

  void rename(std::string newName, std::string oldName = "");

  void save(std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                             &unstagedImages,
            std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
            std::vector<Path> const                          &root);

  bool               hasProjectOpen() const;
  boost::uuids::uuid currentProjectUUID() const;

private:
  std::string name(boost::uuids::uuid uuid);
  Path        path(boost::uuids::uuid uuid);

  ProjectPersistenceListener                           *mListener = nullptr;
  Path                                                  mLocalStatePath;
  Persistence                                           mPersistence;
  std::shared_ptr<Project>                              mProject = nullptr;
  std::optional<boost::uuids::uuid>                     mOpenedUUID;
  boost::bimaps::bimap<boost::uuids::uuid, std::string> mMetadata;
};
} // namespace PB