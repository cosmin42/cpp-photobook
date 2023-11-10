#include <pb/persistence/PersistenceVisitor.h>

namespace PB {
PersistenceVisitor::PersistenceVisitor(
    Path                         centralPersistencePath,
    PersistenceProjectListener  *persistenceProjectListener,
    PersistenceMetadataListener *persistenceMetadataListener)
    : mPersistenceProjectListener(persistenceProjectListener),
      mPersistenceMetadataListener(persistenceMetadataListener),
      mCentral(centralPersistencePath)
{
  auto maybeError = mCentral.connect();
  if (maybeError) {
    mPersistenceMetadataListener->onMetadataPersistenceError(
        maybeError.value());
  }
}

void PersistenceVisitor::persist(Path filePath, ProjectDetails projectDetails)
{
  auto jsonOrError =
      PB::Text::serialize<PB::ProjectDetails>(0, {"root", projectDetails});

  PB::basicAssert(std::holds_alternative<PB::Json>(jsonOrError));

  auto uuidStr = boost::uuids::to_string(projectDetails.uuid());

  PB::FilePersistence newProjectPersistence(filePath);

  newProjectPersistence.write(
      std::get<PB::Json>(jsonOrError).at("root"),
      [this](std::optional<PB::Error> maybeError) {
        if (maybeError) {
          mPersistenceProjectListener->onProjectPersistenceError(
              Error() << ErrorCode::CorruptPersistenceFile);
        }
      });
}

void PersistenceVisitor::persist(ProjectMetadata projectMetadata)
{
  std::pair<std::string, std::string> entry = {
      boost::uuids::to_string(projectMetadata.uuid()),
      projectMetadata.projectFile().string()};

  mCentral.write(entry, [this](std::optional<Error> maybeError) {
    if (maybeError) {
      mPersistenceProjectListener->onProjectPersistenceError(
          Error() << ErrorCode::CannotSaveFile);
    }
  });
}

void PersistenceVisitor::recallMetadata() {}

void PersistenceVisitor::recallProject(Path projectPath)
{
  PB::FilePersistence projectPersistence(projectPath);
  projectPersistence.read(
      [this](std::variant<PB::Json, PB::Error> jsonOrError) {
        auto &json = std::get<PB::Json>(jsonOrError);
        auto  projectDetailsOrError =
            PB::Text::deserialize<PB::ProjectDetails>(json);

        if (std::holds_alternative<PB::Error>(projectDetailsOrError)) {
          mPersistenceProjectListener->onProjectPersistenceError(
              std::get<PB::Error>(projectDetailsOrError));
        }
        else {
          auto &projectDetails =
              std::get<PB::ProjectDetails>(projectDetailsOrError);
          mPersistenceProjectListener->onProjectRead(Project(projectDetails));
        }
      });
}

void PersistenceVisitor::deleteMetadata(std::string id)
{
  mCentral.deleteEntry(id, [this](std::optional<PB::Error>) {
    mPersistenceMetadataListener->onMetadataPersistenceError(
        Error() << ErrorCode::CorruptPersistenceFile);
  });
}

bool PersistenceVisitor::isSaved(Json serialization) const
{
  return serialization == mProjectCache;
}

} // namespace PB