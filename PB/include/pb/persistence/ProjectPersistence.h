#pragma once
#include <pb/persistence/Persistence.h>

namespace PB {
class ProjectPersistenceListener {
public:
  virtual ~ProjectPersistenceListener() = default;
  virtual void onMetadataUpdated() = 0;
  virtual void onProjectRead() = 0;
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

  void onProjectRead(std::shared_ptr<Project> project) override;
  void onProjectPersistenceError(PBDev::Error) override;

  void onMetadataRead(
      boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata) override;
  void onMetadataPersistenceError(PBDev::Error) override;

  void remove(boost::uuids::uuid id);
  void remove(Path path);

  void clear();

private:
  std::string name(boost::uuids::uuid uuid);

  ProjectPersistenceListener *mListener = nullptr;
  Path                        mLocalStatePath;
  Persistence                 mPersistence;
  std::shared_ptr<Project>    mProject = nullptr;

  boost::bimaps::bimap<boost::uuids::uuid, std::string> mMetadata;
};
} // namespace PB