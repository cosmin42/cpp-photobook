#include <pb/persistence/Persistence.h>

#include <unordered_map>

namespace PB {
Persistence::Persistence(
    Path                         applicationLocalStatePath,
    PersistenceProjectListener  *persistenceProjectListener,
    PersistenceMetadataListener *persistenceMetadataListener)
    : mPersistenceProjectListener(persistenceProjectListener),
      mPersistenceMetadataListener(persistenceMetadataListener),
      mCentral(applicationLocalStatePath)
{
  PBDev::basicAssert(persistenceProjectListener != nullptr);
  PBDev::basicAssert(persistenceMetadataListener != nullptr);

  auto maybeError = mCentral.connect();
  if (maybeError && mPersistenceMetadataListener) {
    mPersistenceMetadataListener->onMetadataPersistenceError(
        maybeError.value());
  }
}

void Persistence::setPersistenceListener(
    PersistenceProjectListener  *persistenceProjectListener,
    PersistenceMetadataListener *persistenceMetadataListener)
{
  mPersistenceProjectListener = persistenceProjectListener;
  mPersistenceMetadataListener = persistenceMetadataListener;
}

void Persistence::persistProject(Path filePath, ProjectSnapshot projectDetails)
{

  auto jsonOrError =
      PB::Text::serialize<PB::ProjectSnapshot>(0, {"root", projectDetails});

  PBDev::basicAssert(std::holds_alternative<Json>(jsonOrError));

  auto uuidStr = boost::uuids::to_string(projectDetails.uuid());

  auto supportDirectoryPath =
      projectDetails.parentDirectory() / projectDetails.supportDirName();

  auto maybeError = createSupportDirectory(supportDirectoryPath);

  if (maybeError && mPersistenceProjectListener) {
    mPersistenceProjectListener->onProjectPersistenceError(maybeError.value());
  }

  PB::FilePersistence newProjectPersistence(filePath);

  auto jsonSerialization = std::get<Json>(jsonOrError);

  newProjectPersistence.write(
      jsonSerialization.at("root"),
      [this, jsonSerialization{jsonSerialization}](
          std::optional<PBDev::Error> maybeError) {
        if (maybeError && mPersistenceProjectListener) {
          mPersistenceProjectListener->onProjectPersistenceError(
              PBDev::Error() << ErrorCode::CorruptPersistenceFile);
        }
        else {
          mProjectCache = jsonSerialization;
        }
      });
}

void Persistence::persistMetadata(ProjectMetadata projectMetadata)
{
  std::pair<std::string, std::string> entry = {
      boost::uuids::to_string(projectMetadata.uuid()),
      projectMetadata.projectFile().string()};

  mCentral.write(entry, [this](std::optional<PBDev::Error> maybeError) {
    if (maybeError && mPersistenceProjectListener) {
      mPersistenceProjectListener->onProjectPersistenceError(
          PBDev::Error() << ErrorCode::CannotSaveFile);
    }
  });
}

void Persistence::recallMetadata()
{
  mCentral.read(
      [this](std::variant<std::unordered_map<std::string, std::string>,
                          PBDev::Error>
                 mapOrError) {
        if (std::holds_alternative<PBDev::Error>(mapOrError) &&
            mPersistenceMetadataListener) {
          mPersistenceMetadataListener->onMetadataPersistenceError(
              std::get<PBDev::Error>(mapOrError));
        }
        else {
          auto &map = std::get<std::unordered_map<std::string, std::string>>(
              mapOrError);
          std::vector<ProjectMetadata> projectsMetadata;
          for (auto &[key, value] : map) {
            projectsMetadata.push_back(ProjectMetadata(key, value));
          }
          if (mPersistenceMetadataListener) {
            mPersistenceMetadataListener->onMetadataRead(projectsMetadata);
          }
        }
      });
}

void Persistence::recallProject(Path projectPath)
{
  PB::FilePersistence projectPersistence(projectPath);
  projectPersistence.read(
      [this](std::variant<Json, PBDev::Error> jsonOrError) {
        auto &jsonSerialization = std::get<Json>(jsonOrError);
        auto  projectDetailsOrError =
            PB::Text::deserialize<PB::ProjectSnapshot>(jsonSerialization);

        if (std::holds_alternative<PBDev::Error>(projectDetailsOrError) &&
            mPersistenceProjectListener) {
          mPersistenceProjectListener->onProjectPersistenceError(
              std::get<PBDev::Error>(projectDetailsOrError));
        }
        else {
          auto &projectDetails =
              std::get<PB::ProjectSnapshot>(projectDetailsOrError);
          mProjectCache = jsonSerialization;
          if (mPersistenceProjectListener) {
            mPersistenceProjectListener->onProjectRead(Project(projectDetails));
          }
        }
      });
}

void Persistence::deleteMetadata(std::string id)
{
  mCentral.deleteEntry(id, [this](std::optional<PBDev::Error>) {
    if (mPersistenceMetadataListener) {
      mPersistenceMetadataListener->onMetadataPersistenceError(
          PBDev::Error() << ErrorCode::CorruptPersistenceFile);
    }
  });
}

void Persistence::deleteProject(Path projectFile, Path projectFolder)
{
  std::filesystem::remove(projectFolder);
  std::filesystem::remove(projectFile);
}

bool Persistence::isSaved(ProjectSnapshot const &projectDetails) const
{
  auto jsonOrError =
      PB::Text::serialize<PB::ProjectSnapshot>(0, {"root", projectDetails});

  PBDev::basicAssert(std::holds_alternative<Json>(jsonOrError));

  return std::get<Json>(jsonOrError) == mProjectCache;
}

std::optional<PBDev::Error> Persistence::createSupportDirectory(Path path)
{
  PBDev::basicAssert(!path.string().empty());
  if (std::filesystem::exists(path)) {
    return std::nullopt;
  }

  if (!std::filesystem::create_directory(path)) {
    return PBDev::Error() << ErrorCode::CorruptPersistenceFile;
  }
  return std::nullopt;
}

} // namespace PB