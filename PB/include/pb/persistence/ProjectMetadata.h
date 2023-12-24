#pragma once

#include <variant>

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/util/Error.h>
#include <pb/util/Traits.h>

namespace PB {
class ProjectMetadata {
public:
  static std::variant<std::vector<ProjectMetadata>, PBDev::Error>
  parse(std::variant<std::unordered_map<std::string, std::string>, PBDev::Error>
            mapOrError);

  static std::vector<ProjectMetadata>
  parse(std::unordered_map<std::string, std::string> const &map);

  explicit ProjectMetadata(std::string uuid, std::string path);

  ProjectMetadata() = default;
  ~ProjectMetadata() = default;

  std::pair<std::string, std::string> serialize();

  std::pair<boost::uuids::uuid, Path> data() const;

  Path projectFile() const;
  boost::uuids::uuid uuid() const;

  bool operator==(ProjectMetadata const &other) const;

private:
  boost::uuids::uuid mUUID;
  Path               mProjectFilePath;
};
} // namespace PB