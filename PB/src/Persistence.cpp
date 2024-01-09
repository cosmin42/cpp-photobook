#include <pb/persistence/Persistence.h>

#include <unordered_map>

namespace PB {

void Persistence::configure(Path localStatePath)
{
  mLocalStatePath = localStatePath;
  mCentral.configure(localStatePath);
  auto maybeError = mCentral.connect();
  PBDev::basicAssert(!maybeError.has_value());
}

void Persistence::configure(PersistenceProjectListener *listener)
{
  mPersistenceProjectListener = listener;
}

void Persistence::configure(PersistenceMetadataListener *listener)
{
  mPersistenceMetadataListener = listener;
}

void Persistence::persistProject(Path filePath, ProjectSnapshot projectDetails)
{
  auto jsonOrError =
      PB::Text::serialize<PB::ProjectSnapshot>(0, {"root", projectDetails});

  PBDev::basicAssert(std::holds_alternative<Json>(jsonOrError));

  auto uuidStr = boost::uuids::to_string(projectDetails.uuid);

  auto maybeError =
      createSupportDirectory(ProjectSnapshot::parentDirectory() / "th");

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

void Persistence::persistProject(std::string name, ProjectSnapshot project)
{
  Path projectPath = mLocalStatePath / (name + Context::BOOK_EXTENSION);
  persistProject(projectPath, project);
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
  mCentral.read([this](
                    std::variant<std::unordered_map<std::string, std::string>,
                                 PBDev::Error>
                        mapOrError) {
    if (std::holds_alternative<PBDev::Error>(mapOrError) &&
        mPersistenceMetadataListener) {
      mPersistenceMetadataListener->onMetadataPersistenceError(
          std::get<PBDev::Error>(mapOrError));
    }
    else {
      auto &map =
          std::get<std::unordered_map<std::string, std::string>>(mapOrError);

      boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata;
      for (auto &[key, value] : map) {
        auto               generator = boost::uuids::string_generator();
        boost::uuids::uuid parsedUuid = generator(key);
        metadata.insert({parsedUuid, value});
      }
      if (mPersistenceMetadataListener) {
        mPersistenceMetadataListener->onMetadataRead(metadata);
      }
    }
  });
}

void Persistence::recallProject(Path projectPath)
{
  PB::FilePersistence projectPersistence(projectPath);
  projectPersistence.read([this, name{projectPath.stem().string()}](
                              std::variant<Json, PBDev::Error> jsonOrError) {
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
        mPersistenceProjectListener->onProjectRead(
            std::make_shared<Project>(name, projectDetails));
      }
    }
  });
}

void Persistence::deleteMetadata(std::string id)
{
  mCentral.deleteEntry(id, [this](std::optional<PBDev::Error> maybeError) {
    if (mPersistenceMetadataListener) {
      if (maybeError) {
        mPersistenceMetadataListener->onMetadataPersistenceError(
            PBDev::Error() << ErrorCode::CorruptPersistenceFile);
      }
    }
  });
}

void Persistence::deleteProject(Path projectFile)
{
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