#include <pb/persistence/ProjectMetadata.h>

namespace PB {
ProjectMetadata::ProjectMetadata(std::string uuid, std::string path)
{
  try {
    boost::uuids::string_generator gen;
    mUUID = gen(uuid);
  }
  catch (...) {
    PB::basicAssert(false);
  }
  mProjectFilePath = path;
}

std::pair<std::string, std::string> ProjectMetadata::serialize()
{
  std::pair<std::string, std::string> result = {boost::uuids::to_string(mUUID),
                                                mProjectFilePath.string()};
  return result;
}

std::variant<std::vector<ProjectMetadata>, Error> ProjectMetadata::parse(
    std::variant<std::unordered_map<std::string, std::string>, Error>
        mapOrError)
{
  if (std::holds_alternative<Error>(mapOrError)) {
    return std::get<Error>(mapOrError);
  }

  auto &map =
      std::get<std::unordered_map<std::string, std::string>>(mapOrError);
  return parse(map);
}

std::vector<ProjectMetadata>
ProjectMetadata::parse(std::unordered_map<std::string, std::string> const &map)
{
  std::vector<ProjectMetadata> result;

  for (auto &[key, value] : map) {
    result.push_back(ProjectMetadata(key, value));
  }

  return result;
}

std::pair<boost::uuids::uuid, Path> ProjectMetadata::data() const
{
  return {mUUID, mProjectFilePath};
}

Path ProjectMetadata::projectFile() const { return mProjectFilePath; }

boost::uuids::uuid ProjectMetadata::uuid() const { return mUUID; }
} // namespace PB