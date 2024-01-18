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

void Persistence::persistProject(
    Path filePath, ProjectSnapshot projectDetails,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  auto jsonOrError =
      PB::Text::serialize<PB::ProjectSnapshot>(0, {"root", projectDetails});

  PBDev::basicAssert(std::holds_alternative<Json>(jsonOrError));

  auto imageJsonOrError = PB::Text::serialize<
      std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
      0, {"unstaged", unstagedImages});

  PBDev::basicAssert(std::holds_alternative<Json>(imageJsonOrError));

  std::get<Json>(jsonOrError).update(std::get<Json>(imageJsonOrError));

  imageJsonOrError =
      PB::Text::serialize<std::vector<std::shared_ptr<VirtualImage>>>(
          0, {"staged", stagedImages});

  PBDev::basicAssert(std::holds_alternative<Json>(imageJsonOrError));

  std::get<Json>(jsonOrError).update(std::get<Json>(imageJsonOrError));

  imageJsonOrError =
      PB::Text::serialize<std::vector<Path>>(0, {"row-paths", roots});

  PBDev::basicAssert(std::holds_alternative<Json>(imageJsonOrError));

  std::get<Json>(jsonOrError).update(std::get<Json>(imageJsonOrError));

  auto maybeError =
      createSupportDirectory(ProjectSnapshot::parentDirectory() / "th");

  if (maybeError && mPersistenceProjectListener) {
    mPersistenceProjectListener->onProjectPersistenceError(maybeError.value());
  }

  PB::FilePersistence newProjectPersistence(filePath);

  auto jsonSerialization = std::get<Json>(jsonOrError);

  newProjectPersistence.write(
      jsonSerialization, [this, jsonSerialization{jsonSerialization}](
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

void Persistence::persistProject(
    std::string name, ProjectSnapshot project,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  Path projectPath = mLocalStatePath / (name + Context::BOOK_EXTENSION);
  persistProject(projectPath, project, unstagedImages, stagedImages, roots);
}

void Persistence::persistMetadata(boost::uuids::uuid const &id,
                                  std::string               name)
{
  std::pair<std::string, std::string> entry = {boost::uuids::to_string(id),
                                               name};

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
      return;
    }

    auto &projectDetails = std::get<PB::ProjectSnapshot>(projectDetailsOrError);

    auto unstagedImagesOrError = PB::Text::deserialize<
        std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
        jsonSerialization.at("unstaged"));

    std::vector<std::shared_ptr<VirtualImage>> stagedImages;

    mProjectCache = jsonSerialization;
    if (mPersistenceProjectListener) {
      mPersistenceProjectListener->onProjectRead(
          name, std::make_shared<Project>(projectDetails),
          std::get<std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
              unstagedImagesOrError),
          stagedImages);
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