#include <pb/persistence/Persistence.h>

#include <pb/persistence/SerializationStrategy.h>

#include <unordered_map>

namespace PB {

void Persistence::configure(Path localStatePath)
{
  mLocalStatePath = localStatePath;
  mCentral.configure(localStatePath);
  mCentral.configureSQLiteListener((SQLitePersistenceListener *)this);
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

Json Persistence::serialization(
    Project projectDetails,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  auto jsonOrError =
      PB::Text::serialize<PB::Project>(0, {"root", projectDetails});

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

  return std::get<Json>(jsonOrError);
}

void Persistence::persistProject(Path localInstallFolder, Path filePath,
                                 Json        jsonSerialization,
                                 std::string thumbnailsDirectoryName)
{
  auto maybeError = createSupportDirectory(localInstallFolder / "th",
                                           thumbnailsDirectoryName);

  if (maybeError && mPersistenceProjectListener) {
    mPersistenceProjectListener->onProjectPersistenceError(maybeError.value());
  }

  PB::FilePersistence newProjectPersistence(filePath);

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

void Persistence::persistProject(Path localInstallFolder, std::string name,
                                 Json json, std::string thumbnailsDirectoryName)
{
  Path projectPath = mLocalStatePath / (name + OneConfig::BOOK_EXTENSION);
  persistProject(localInstallFolder, projectPath, json,
                 thumbnailsDirectoryName);
}

void Persistence::persistMetadata(boost::uuids::uuid const &id,
                                  std::string               name)
{
  std::pair<std::string, std::string> entry = {boost::uuids::to_string(id),
                                               name};

  mCentral.write(entry);
}

void Persistence::recallMetadata() { mCentral.read(); }

void Persistence::recallProject(Path projectPath)
{
  PB::FilePersistence projectPersistence(projectPath);
  projectPersistence.read([this, name{projectPath.stem().string()}](
                              std::variant<Json, PBDev::Error> jsonOrError) {
    auto &jsonSerialization = std::get<Json>(jsonOrError);
    auto  projectDetailsOrError =
        PB::Text::deserialize<PB::Project>(jsonSerialization);

    if (std::holds_alternative<PBDev::Error>(projectDetailsOrError) &&
        mPersistenceProjectListener) {
      mPersistenceProjectListener->onProjectPersistenceError(
          std::get<PBDev::Error>(projectDetailsOrError));
      return;
    }

    auto &projectDetails = std::get<PB::Project>(projectDetailsOrError);

    auto unstagedImagesOrError = PB::Text::deserialize<
        std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
        jsonSerialization.at("unstaged"));

    if (std::holds_alternative<PBDev::Error>(unstagedImagesOrError) &&
        mPersistenceProjectListener) {
      mPersistenceProjectListener->onProjectPersistenceError(
          std::get<PBDev::Error>(unstagedImagesOrError));
      return;
    }

    auto stagedImagesOrError =
        PB::Text::deserialize<std::vector<std::shared_ptr<VirtualImage>>>(
            jsonSerialization.at("staged"));

    if (std::holds_alternative<PBDev::Error>(stagedImagesOrError) &&
        mPersistenceProjectListener) {
      mPersistenceProjectListener->onProjectPersistenceError(
          std::get<PBDev::Error>(stagedImagesOrError));
      return;
    }
    std::variant<std::vector<Path>, PBDev::Error> importedFoldersOrError;
#ifndef _CLANG_UML_
    importedFoldersOrError = PB::Text::deserializeSpecial(
        jsonSerialization, "row-paths");

    if (std::holds_alternative<PBDev::Error>(importedFoldersOrError) &&
        mPersistenceProjectListener) {
      mPersistenceProjectListener->onProjectPersistenceError(
          std::get<PBDev::Error>(importedFoldersOrError));
      return;
    }
#endif

    mProjectCache = jsonSerialization;
#ifndef _CLANG_UML_
    if (mPersistenceProjectListener) {
      mPersistenceProjectListener->onJsonRead(jsonSerialization);
      mPersistenceProjectListener->onProjectRead(
          name, std::make_shared<Project>(projectDetails),
          std::get<std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
              unstagedImagesOrError),
          std::get<std::vector<std::shared_ptr<VirtualImage>>>(
              stagedImagesOrError),
          std::get<std::vector<Path>>(importedFoldersOrError));
    }
#endif
  });
}

void Persistence::deleteMetadata(std::string id) { mCentral.deleteEntry(id); }

void Persistence::deleteProject(Path               projectFile,
                                std::string        thumbnailsDirectoryName,
                                boost::uuids::uuid id)
{
  mCentral.deleteEntry(projectFile.stem().string());
  mCentral.deleteHash(boost::uuids::to_string(id));
  auto projectName = projectFile.stem().string();
  auto thumbnailsPath = mLocalStatePath / "th" / thumbnailsDirectoryName;
  std::filesystem::remove_all(thumbnailsPath);
  std::filesystem::remove(projectFile);
}

std::string Persistence::hash(Path path, boost::uuids::uuid id)
{
  return mCentral.hash(path, boost::uuids::to_string(id));
}

boost::bimaps::bimap<Path, std::string>
Persistence::hashSet(boost::uuids::uuid id)
{
  return mCentral.hashSet(boost::uuids::to_string(id));
}

void Persistence::onSQLiteMetadataRead(
    std::unordered_map<std::string, std::string> map)
{
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
void Persistence::onSQLiteMetadataWritten() {}
void Persistence::onSQLiteEntryDeleted() {}
void Persistence::onSQLiteMetadataError(PBDev::Error error)
{
  mPersistenceMetadataListener->onMetadataPersistenceError(error);
}

std::optional<PBDev::Error>
Persistence::createSupportDirectory(Path        path,
                                    std::string thumbnailDirectoryName)
{
  PBDev::basicAssert(!path.string().empty());

  if (!std::filesystem::exists(path)) {
    if (!std::filesystem::create_directory(path)) {
      return PBDev::Error() << ErrorCode::CorruptPersistenceFile;
    }
  }

  if (std::filesystem::exists(path / thumbnailDirectoryName)) {
    return std::nullopt;
  }

  if (!std::filesystem::create_directory(path / thumbnailDirectoryName)) {
    return PBDev::Error() << ErrorCode::CorruptPersistenceFile;
  }

  return std::nullopt;
}

} // namespace PB